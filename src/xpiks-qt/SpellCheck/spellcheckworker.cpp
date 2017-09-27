/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "spellcheckworker.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QThread>
#include "spellcheckitem.h"
#include "../Common/defines.h"
#include "../Common/flags.h"
#include "../Helpers/stringhelper.h"
#include <hunspell/hunspell.hxx>

#define EN_HUNSPELL_DIC "en_US.dic"
#define EN_HUNSPELL_AFF "en_US.aff"

#define MINIMUM_LENGTH_FOR_STEMMING 3

namespace SpellCheck {
    SpellCheckWorker::SpellCheckWorker(Helpers::AsyncCoordinator *initCoordinator, Models::SettingsModel *settingsModel, QObject *parent):
        QObject(parent),
        m_InitCoordinator(initCoordinator),
        m_SettingsModel(settingsModel),
        m_Hunspell(NULL),
        m_Codec(NULL),
        m_UserDictionaryPath("")
    {
        Q_ASSERT(settingsModel);
    }

    SpellCheckWorker::~SpellCheckWorker() {
        if (m_Hunspell != NULL) {
            delete m_Hunspell;
        }

        LOG_INFO << "destroyed";
    }

    bool SpellCheckWorker::initWorker() {
        LOG_INFO << "#";

        Helpers::AsyncCoordinatorUnlocker unlocker(m_InitCoordinator);
        Q_UNUSED(unlocker);

        QString resourcesPath;
        QString affPath;
        QString dicPath;

#if !defined(Q_OS_LINUX)
        resourcesPath = QCoreApplication::applicationDirPath();
        LOG_DEBUG << "App path:" << resourcesPath;

#  if defined(Q_OS_MAC)
        resourcesPath += "/../Resources/";
#  elif defined(APPVEYOR)
        resourcesPath += "/../../../xpiks-qt/deps/dict/";
#  elif defined(Q_OS_WIN)
        resourcesPath += "/dict/";
#  endif

        QDir resourcesDir(resourcesPath);
        affPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_AFF);
        dicPath = resourcesDir.absoluteFilePath(EN_HUNSPELL_DIC);

#else
#if defined(TRAVIS_CI)
        resourcesPath = STRINGIZE(HUNSPELL_DICTS_PATH);
        LOG_DEBUG << "Resources path:" << resourcesPath;
#else
        if (m_SettingsModel != nullptr) {
            resourcesPath = m_SettingsModel->getDictPath();
        }
#endif

        if (resourcesPath.isEmpty()) {
            resourcesPath = "hunspell/";
            dicPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, resourcesPath + EN_HUNSPELL_DIC);
            affPath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, resourcesPath + EN_HUNSPELL_AFF);
        } else {
            QDir resourcesDirectory(resourcesPath);
            affPath = resourcesDirectory.absoluteFilePath(EN_HUNSPELL_AFF);
            dicPath = resourcesDirectory.absoluteFilePath(EN_HUNSPELL_DIC);
        }

#endif

        bool initResult = false;

        if (QFileInfo(affPath).exists() && QFileInfo(dicPath).exists()) {
#ifdef Q_OS_WIN
            // specific Hunspell handling of UTF-8 encoded pathes
            affPath = "\\\\?\\" + QDir::toNativeSeparators(affPath);
            dicPath = "\\\\?\\" + QDir::toNativeSeparators(dicPath);
#endif

            try {
                m_Hunspell = new Hunspell(affPath.toUtf8().constData(),
                                          dicPath.toUtf8().constData());
                LOG_DEBUG << "Hunspell initialized with AFF" << affPath << "and DIC" << dicPath;
                initResult = true;
                m_Encoding = QString::fromLatin1(m_Hunspell->get_dic_encoding());
                m_Codec = QTextCodec::codecForName(m_Encoding.toLatin1().constData());
            } catch (...) {
                LOG_DEBUG << "Error in Hunspell with AFF" << affPath << "and DIC" << dicPath;
                m_Hunspell = NULL;
            }
        } else {
            LOG_WARNING << "DIC or AFF file not found." << dicPath << "||" << affPath;
        }

        initUserDictionary();

        return initResult;
    }

    void SpellCheckWorker::processOneItem(std::shared_ptr<ISpellCheckItem> &item) {
        auto separatorItem = std::dynamic_pointer_cast<SpellCheckSeparatorItem>(item);
        auto queryItem = std::dynamic_pointer_cast<SpellCheckItem>(item);
        auto addWordItem = std::dynamic_pointer_cast<ModifyUserDictItem>(item);

        if (queryItem) {
            processQueryItem(queryItem);
        } else if (separatorItem) {
            processSeparatorItem(separatorItem);
        } else if (addWordItem) {
            processChangeUserDict(addWordItem);
        }
    }

    void SpellCheckWorker::processSeparatorItem(std::shared_ptr<SpellCheckSeparatorItem> &item) {
        Q_UNUSED(item);
        emit queueIsEmpty();
    }

    void SpellCheckWorker::processQueryItem(std::shared_ptr<SpellCheckItem> &item) {
        const bool neededSuggestions = item->needsSuggestions();
        auto &queryItems = item->getQueries();
        const auto wordAnalysisFlags = item->getWordAnalysisFlags();
        const bool shouldCheckSpelling = Common::HasFlag(wordAnalysisFlags, Common::WordAnalysisFlags::Spelling);
        const bool shouldStemWord = Common::HasFlag(wordAnalysisFlags, Common::WordAnalysisFlags::Stemming);
        bool anyWrong = false;

        if (!neededSuggestions) {
            const size_t size = queryItems.size();
            for (size_t i = 0; i < size; ++i) {
                auto &queryItem = queryItems.at(i);
                bool isOk = true;

                if (shouldCheckSpelling) {
                    isOk = checkWordSpelling(queryItem);
                }

                if (shouldStemWord) {
                    stemWord(queryItem);
                }

                anyWrong = anyWrong || !isOk;
            }

            if (shouldStemWord && !item->getIsOnlyOneKeyword()) {
                findSemanticDuplicates(queryItems);
            }

            item->accountResults();
            item->submitSpellCheckResult();
        } else {
            for (auto &queryItem: queryItems) {
                if (!queryItem->m_IsCorrect) {
                    findSuggestions(queryItem->m_Word);
                }
            }
        }

        if (anyWrong) {
            item->requestSuggestions();
            this->submitItem(item);
        }
    }

    void SpellCheckWorker::processChangeUserDict(std::shared_ptr<ModifyUserDictItem> &item) {
        LOG_INTEGRATION_TESTS << item->getKeywordsToAdd();

        if (m_UserDictionaryPath.isEmpty()) {
            LOG_WARNING << "User dictionary not set.";
        }

        auto clearflag = item->getClearFlag();
        QStringList words = item->getKeywordsToAdd();

        if (clearflag && words.empty()) { //clear dict
            cleanUserDict();
        } else if (!clearflag && !words.empty()) {//append
            changeUserDict(words, false);
        } else if (clearflag && !words.empty()) { //overwrite
            changeUserDict(words, true);
        }

        signalUserDictWordsCount();
    }

    QStringList SpellCheckWorker::retrieveCorrections(const QString &word) {
        QReadLocker locker(&m_SuggestionsLock);
        QStringList result;

        auto it = m_Suggestions.find(word);
        if (it != m_Suggestions.end()) {
            result = it.value();
        }

        return result;
    }

    QStringList SpellCheckWorker::suggestCorrections(const QString &word) {
        QStringList suggestions;
        std::vector<std::string> suggestWordList;

        try {
            // Encode from Unicode to the encoding used by current dictionary
            std::string encodedWord = m_Codec->fromUnicode(word).toStdString();
            suggestWordList = m_Hunspell->suggest(encodedWord);
            LOG_INTEGRATION_TESTS << "Found" << suggestWordList.size() << "suggestions for" << word;
            QString lowerWord = word.toLower();

            for (size_t i = 0; i < suggestWordList.size(); ++i) {
                QString suggestion = m_Codec->toUnicode(QByteArray::fromStdString(suggestWordList[i]));

                if (suggestion.toLower() != lowerWord) {
                    suggestions << suggestion;
                }
            }
        } catch (...) {
            LOG_WARNING << "Error for keyword:" << word;
        }
        return suggestions;
    }

    bool SpellCheckWorker::checkWordSpelling(const std::shared_ptr<SpellCheckQueryItem> &queryItem) {
        bool isOk = false;

        const QString &word = queryItem->m_Word;
        const bool isInUserDict = m_UserDictionary.contains(word);

        isOk = isInUserDict || checkWordSpelling(word);
        queryItem->m_IsCorrect = isOk;

        return isOk;
    }

    bool SpellCheckWorker::checkWordSpelling(const QString &word) {
        bool isOk = false;

        const bool isCached = m_WrongWords.contains(word);

        if (!isCached) {
            isOk = isHunspellSpellingCorrect(word);

            if (!isOk) {
                QString capitalized = word;
                capitalized[0] = capitalized[0].toUpper();

                if (isHunspellSpellingCorrect(capitalized)) {
                    isOk = true;
                }
            }
        }

        if (!isOk) {
            m_WrongWords.insert(word);
        }

        return isOk;
    }

    QString SpellCheckWorker::getWordStem(const QString &word) {
        QString result;

        if (word.isEmpty()) { return result; }

        std::string encodedWord = m_Codec->fromUnicode(word).toStdString();
        std::vector<std::string> stems;

        try {
            stems = m_Hunspell->stem(encodedWord);
        } catch(...) {
            return result;
        }

#ifdef INTEGRATION_TESTS
        QStringList stemsList;
        for (auto &stm: stems) { stemsList.append(QString::fromStdString(stm)); }
        LOG_DEBUG << "Stemming:" << word << "->" << stemsList;
#endif

        const size_t size = stems.size();
        if (size > 0) {
            size_t minIndex = 0, minSize = stems[0].length();

            for (size_t i = 1; i < size; i++) {
                const size_t currSize = stems[i].length();
                if (currSize < minSize) {
                    minSize = currSize;
                    minIndex = i;
                }
            }

            result = QString::fromStdString(stems[minIndex]);
        }

        return result;
    }

    void SpellCheckWorker::stemWord(const std::shared_ptr<SpellCheckQueryItem> &queryItem) {
        QString word = queryItem->m_Word;
        if (word.length() >= MINIMUM_LENGTH_FOR_STEMMING) {
            queryItem->m_Stem = getWordStem(word.toLower());
        }
    }

    bool SpellCheckWorker::isHunspellSpellingCorrect(const QString &word) const {
        bool isOk = false;

        try {
            std::string encodedWord = m_Codec->fromUnicode(word).toStdString();
            isOk = m_Hunspell->spell(encodedWord) != 0;
        } catch (...) {
            isOk = false;
        }
        return isOk;
    }

    void SpellCheckWorker::findSemanticDuplicates(const std::vector<std::shared_ptr<SpellCheckQueryItem> > &queries) {
        LOG_INTEGR_TESTS_OR_DEBUG << "#";
        const size_t size = queries.size();

        QHash<QString, QVector<size_t> > stemToIndexMap;

        for (size_t i = 0; i < size; ++i) {
            auto &queryItem = queries.at(i);
            if (queryItem->m_Stem.isEmpty()) {
                LOG_WARNING << "Stem is empty for" << queryItem->m_Word;
                continue;
            }

            stemToIndexMap[queryItem->m_Stem].append(i);
        }

        LOG_INTEGR_TESTS_OR_DEBUG << "Stems hash has" << stemToIndexMap.size() << "item(s)";
        LOG_INTEGRATION_TESTS << stemToIndexMap;

        auto itEnd = stemToIndexMap.constEnd();
        auto it = stemToIndexMap.constBegin();

        while (it != itEnd) {
            const QVector<size_t> &sameStemIndices = it.value();
            if (sameStemIndices.size() > 1) {
                const int size = sameStemIndices.size();

                for (int i = 0; i < size; i++) {
                    for (int j = 0; j < size; j++) {
                        if (i == j) { continue; }

                        size_t index1 = sameStemIndices[i];
                        size_t index2 = sameStemIndices[j];

                        auto &query1 = queries[index1];
                        auto &query2 = queries[index2];
                        Q_ASSERT(query1->m_Stem == query2->m_Stem);

                        if (query1->m_IsDuplicate && query2->m_IsDuplicate) { continue; }

                        if (Helpers::areSemanticDuplicates(query1->m_Word, query2->m_Word)) {
                            LOG_INTEGR_TESTS_OR_DEBUG << "detected as duplicates:" << "[" << query1->m_Index << "]:" << query1->m_Word << "[" << query2->m_Index << "]:" << query2->m_Word;
                            query1->m_IsDuplicate = true;
                            query2->m_IsDuplicate = true;
                        }
                    }
                }
            }

            ++it;
        }
    }

    void SpellCheckWorker::findSuggestions(const QString &word) {
        LOG_INTEGRATION_TESTS << word;
        bool needsCorrections = false;

        m_SuggestionsLock.lockForRead();
        needsCorrections = !m_Suggestions.contains(word);
        m_SuggestionsLock.unlock();

        if (needsCorrections) {
            QStringList suggestions = suggestCorrections(word);
            m_SuggestionsLock.lockForWrite();
            m_Suggestions.insert(word, suggestions);
            m_SuggestionsLock.unlock();
        }
    }

    void SpellCheckWorker::initUserDictionary() {
        LOG_DEBUG << "#";
        QString appDataPath = XPIKS_USERDATA_PATH;
        QDir dir(appDataPath);

        m_UserDictionaryPath = dir.filePath(QLatin1String(Constants::USER_DICT_FILENAME));
        QFile userDictonaryFile(m_UserDictionaryPath);

        if (userDictonaryFile.open(QIODevice::ReadOnly)) {
            QTextStream stream(&userDictonaryFile);
            for (QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine()) {
                m_UserDictionary.addWord(word);
            }

            signalUserDictWordsCount();
            if (!m_UserDictionary.empty()) {
                emit userDictUpdate(m_UserDictionary.getWords(), false);
            }
        } else {
            LOG_WARNING << "Cannot open" << m_UserDictionaryPath;
        }

        LOG_INFO << "User Dictionary contains:" << m_UserDictionary.size() << "item(s)";
    }


    void SpellCheckWorker::cleanUserDict() {
        LOG_DEBUG << "#";

        m_UserDictionary.clear();
        emit userDictCleared();

        QFile userDictonaryFile(m_UserDictionaryPath);
        if (userDictonaryFile.open(QIODevice::ReadWrite)) {
            userDictonaryFile.resize(0);
        } else {
            LOG_INFO << "Unable to trunkate user dictionary file:" << m_UserDictionaryPath;
        }
    }

    void SpellCheckWorker::changeUserDict(const QStringList &words, bool overwrite) {
        LOG_INFO << "Words to add:" << words;

        QStringList wordsToAdd;

        for (auto &word: words) {
            const bool isOk = checkWordSpelling(word);
            if (overwrite || !isOk) {
                wordsToAdd.append(word);
            }
        }

        LOG_INTEGRATION_TESTS << "Real words to add:" << wordsToAdd;

        if (overwrite) {
            m_UserDictionary.clear();
        }

        m_UserDictionary.addWords(wordsToAdd);

        emit userDictUpdate(wordsToAdd, overwrite);

        QFile userDictonaryFile(m_UserDictionaryPath);
        auto mode = overwrite? QIODevice::WriteOnly : QIODevice::Append;
        if (userDictonaryFile.open(mode)) {
            QTextStream stream(&userDictonaryFile);

            for (const QString &word: wordsToAdd) {
                stream << word << endl;
            }
        } else {
            LOG_WARNING << "Unable to open user dictionary";
        }
    }

    void SpellCheckWorker::signalUserDictWordsCount() {
        LOG_DEBUG << m_UserDictionary.size();
        emit wordsNumberChanged(m_UserDictionary.size());
    }
}
