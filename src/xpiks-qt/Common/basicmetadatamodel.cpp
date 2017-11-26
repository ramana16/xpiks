/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "basicmetadatamodel.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordshelpers.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"

namespace Common {
    BasicMetadataModel::BasicMetadataModel(Hold &hold, QObject *parent):
        BasicKeywordsModel(hold, parent),
        m_SpellCheckInfo(NULL)
    { }

    QString BasicMetadataModel::getDescription() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description;
    }

    QString BasicMetadataModel::getTitle() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title;
    }

#ifdef CORE_TESTS
    void BasicMetadataModel::initialize(const QString &title, const QString &description, const QString &rawKeywords) {
        setTitle(title);
        setDescription(description);
        setKeywords(rawKeywords.split(',', QString::SkipEmptyParts));
    }
#endif

    void BasicMetadataModel::setSpellCheckResults(const QHash<QString, Common::WordAnalysisResult> &results, Common::SpellCheckFlags flags) {
        const bool withStems = flags == Common::SpellCheckFlags::All;

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Description)) {
            updateDescriptionSpellErrors(results, withStems);
        }

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Title)) {
            updateTitleSpellErrors(results, withStems);
        }
    }

    QStringList BasicMetadataModel::retrieveMisspelledDescriptionWords() {
        LOG_DEBUG << "#";
        QStringList descriptionWords = getDescriptionWords();
        const int length = descriptionWords.length();

        QStringList misspelledWords;
        misspelledWords.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = descriptionWords.at(i);
            if (m_SpellCheckInfo->hasDescriptionError(word.toLower())) {
                LOG_DEBUG << word << "has wrong spelling";

                misspelledWords.append(word);
            }
        }

        return misspelledWords;
    }

    QStringList BasicMetadataModel::retrieveMisspelledTitleWords() {
        LOG_DEBUG << "#";
        QStringList titleWords = getTitleWords();
        const int length = titleWords.length();

        QStringList misspelledWords;
        misspelledWords.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = titleWords.at(i);
            if (m_SpellCheckInfo->hasTitleError(word.toLower())) {
                LOG_DEBUG << word << "has wrong spelling";

                misspelledWords.append(word);
            }
        }

        return misspelledWords;
    }

    bool BasicMetadataModel::fixDescriptionSpelling(const QString &word, const QString &replacement) {
        Common::SearchFlags flags = Common::SearchFlags::None;
        Common::SetFlag(flags, Common::SearchFlags::CaseSensitive);
        Common::SetFlag(flags, Common::SearchFlags::Description);

        bool result = replaceInDescription(word, replacement, flags);
        return result;
    }

    bool BasicMetadataModel::fixTitleSpelling(const QString &word, const QString &replacement) {
        Common::SearchFlags flags = Common::SearchFlags::None;
        Common::SetFlag(flags, Common::SearchFlags::CaseSensitive);
        Common::SetFlag(flags, Common::SearchFlags::Description);

        bool result = replaceInTitle(word, replacement, flags);
        return result;
    }

    void BasicMetadataModel::setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
        BasicKeywordsModel::setKeywordsSpellCheckResults(items);
    }

    bool BasicMetadataModel::processFailedKeywordReplacements(const std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval) {
        return BasicKeywordsModel::processFailedKeywordReplacements(candidatesForRemoval);
    }

    std::vector<KeywordItem> BasicMetadataModel::retrieveMisspelledKeywords() {
        return BasicKeywordsModel::retrieveMisspelledKeywords();
    }

    KeywordReplaceResult BasicMetadataModel::fixKeywordSpelling(size_t index, const QString &existing, const QString &replacement) {
        return BasicKeywordsModel::fixKeywordSpelling(index, existing, replacement);
    }

    void BasicMetadataModel::afterReplaceCallback() {
        BasicKeywordsModel::afterReplaceCallback();
    }

    BasicKeywordsModel *BasicMetadataModel::getBasicKeywordsModel() {
        return this;
    }

    QStringList BasicMetadataModel::getDescriptionWords() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Description, words);
        return words;
    }

    QStringList BasicMetadataModel::getTitleWords() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Title, words);
        return words;
    }

    bool BasicMetadataModel::expandPreset(size_t keywordIndex, const QStringList &presetList) {
        return BasicKeywordsModel::expandPreset(keywordIndex, presetList);
    }

    bool BasicMetadataModel::appendPreset(const QStringList &presetList) {
        return BasicKeywordsModel::appendPreset(presetList);
    }

    bool BasicMetadataModel::replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                                  Common::SearchFlags flags) {
        LOG_DEBUG << "#";
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::WholeWords);
        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QString description = getDescription();
        if (!wholeWords) {
            description.replace(replaceWhat, replaceTo, caseSensivity);
        } else {
            description = Helpers::replaceWholeWords(description, replaceWhat, replaceTo, caseSensivity);
        }

        bool result = setDescription(description);
        return result;
    }

    bool BasicMetadataModel::replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                                            Common::SearchFlags flags) {
        LOG_DEBUG << "#";
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::WholeWords);
        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        QString title = getTitle();
        if (!wholeWords) {
            title.replace(replaceWhat, replaceTo, caseSensivity);
        } else {
            title = Helpers::replaceWholeWords(title, replaceWhat, replaceTo, caseSensivity);
        }

        bool result = setTitle(title);
        return result;
    }

    bool BasicMetadataModel::replace(const QString &replaceWhat, const QString &replaceTo, SearchFlags flags) {
        LOG_INFO << "[" << replaceWhat << "] -> [" << replaceTo << "] with flags:" << (int)flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(((int)flags & (int)Common::SearchFlags::Metadata) != 0);
        bool anyChanged = BasicKeywordsModel::replace(replaceWhat, replaceTo, flags);

        const bool needToCheckDescription = Common::HasFlag(flags, Common::SearchFlags::Description);
        if (needToCheckDescription) {
            if (this->replaceInDescription(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        const bool needToCheckTitle = Common::HasFlag(flags, Common::SearchFlags::Title);
        if (needToCheckTitle) {
            if (this->replaceInTitle(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        return anyChanged;
    }

    bool BasicMetadataModel::hasDescriptionSpellError() {
        bool anyError = false;

        const QStringList &descriptionWords = getDescriptionWords();

        foreach(const QString &word, descriptionWords) {
            if (m_SpellCheckInfo->hasDescriptionError(word.toLower())) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicMetadataModel::hasTitleSpellError() {
        bool anyError = false;

        const QStringList &titleWords = getTitleWords();

        foreach(const QString &word, titleWords) {
            if (m_SpellCheckInfo->hasTitleError(word.toLower())) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicMetadataModel::hasDescriptionWordSpellError(const QString &word) {
        return m_SpellCheckInfo->hasDescriptionError(word.toLower());
    }

    bool BasicMetadataModel::hasTitleWordSpellError(const QString &word) {
        return m_SpellCheckInfo->hasTitleError(word.toLower());
    }

    bool BasicMetadataModel::hasSpellErrors() {
        bool result = hasDescriptionSpellError() ||
                hasTitleSpellError() ||
                BasicKeywordsModel::hasSpellErrors();

        return result;
    }

    bool BasicMetadataModel::hasDuplicates() {
        bool result = m_SpellCheckInfo->anyDescriptionDuplicates() ||
                m_SpellCheckInfo->anyTitleDuplicates() ||
                BasicKeywordsModel::hasDuplicates();
        return result;
    }

    bool BasicMetadataModel::setDescription(const QString &value) {
        QWriteLocker writeLocker(&m_DescriptionLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Description;
        if (result) {
            m_Description = value;
        }

        return result;
    }

    bool BasicMetadataModel::setTitle(const QString &value) {
        QWriteLocker writeLocker(&m_TitleLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Title;
        if (result) {
            m_Title = value;
        }

        return result;
    }

    bool BasicMetadataModel::isEmpty() {
        bool isEmpty = BasicKeywordsModel::isEmpty();

        if (!isEmpty) {
            QReadLocker readDescriptionLock(&m_DescriptionLock);
            Q_UNUSED(readDescriptionLock);
            isEmpty = m_Description.trimmed().isEmpty();
        }

        return isEmpty;
    }

    bool BasicMetadataModel::isTitleEmpty() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title.trimmed().isEmpty();
    }

    bool BasicMetadataModel::isDescriptionEmpty() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description.trimmed().isEmpty();
    }

    void BasicMetadataModel::clearModel() {
        setDescription("");
        setTitle("");
        clearKeywords();
    }

    void BasicMetadataModel::notifySpellCheckResults(SpellCheckFlags flags) {
        LOG_INTEGR_TESTS_OR_DEBUG << (int)flags;

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Description)) {
            notifyDescriptionSpellingChanged();
        }

        if (Common::HasFlag(flags, Common::SpellCheckFlags::Title)) {
            notifyTitleSpellingChanged();
        }

        BasicKeywordsModel::notifySpellCheckResults(flags);
    }

    void BasicMetadataModel::notifyDescriptionSpellingChanged() {
        emit descriptionSpellingChanged();
    }

    void BasicMetadataModel::notifyTitleSpellingChanged() {
        emit titleSpellingChanged();
    }

    void BasicMetadataModel::updateDescriptionSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results, bool withStemInfo) {
        QSet<QString> descriptionErrors;
        QSet<QString> descriptionDuplicates;
        QStringList descriptionWords = getDescriptionWords();
        Common::WordAnalysisResult defaultWordAnalysisResult;

        foreach(const QString &word, descriptionWords) {
            Common::WordAnalysisResult wordResult = results.value(word, defaultWordAnalysisResult);
            const QString lowerWord = word.toLower();

            if (wordResult.m_IsCorrect == false) {
                descriptionErrors.insert(lowerWord);
            }

            if (withStemInfo && wordResult.m_HasDuplicates) {
                descriptionDuplicates.insert(lowerWord);
            }
        }

        m_SpellCheckInfo->setDescriptionErrors(descriptionErrors);

        if (withStemInfo) {
            m_SpellCheckInfo->setDescriptionDuplicates(descriptionDuplicates);
        }
    }

    void BasicMetadataModel::updateTitleSpellErrors(const QHash<QString, Common::WordAnalysisResult> &results, bool withStemInfo) {
        QSet<QString> titleErrors;
        QSet<QString> titleDuplicates;
        QStringList titleWords = getTitleWords();
        Common::WordAnalysisResult defaultWordAnalysisResult;

        foreach(const QString &word, titleWords) {
            Common::WordAnalysisResult wordResult = results.value(word, defaultWordAnalysisResult);
            const QString lowerWord = word.toLower();

            if (wordResult.m_IsCorrect == false) {
                titleErrors.insert(lowerWord);
            }

            if (withStemInfo && wordResult.m_HasDuplicates) {
                titleDuplicates.insert(lowerWord);
            }
        }

        m_SpellCheckInfo->setTitleErrors(titleErrors);

        if (withStemInfo) {
            m_SpellCheckInfo->setTitleDuplicates(titleDuplicates);
        }
    }
}
