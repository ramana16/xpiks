/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKWORKER_H
#define SPELLCHECKWORKER_H

#include <QString>
#include <QStringList>
#include <QReadWriteLock>
#include <QHash>
#include <QSet>
#include "../Common/itemprocessingworker.h"
#include "../Models/settingsmodel.h"
#include "spellcheckitem.h"
#include "../Helpers/asynccoordinator.h"

class Hunspell;
class QTextCodec;

namespace SpellCheck {
    class UserDictionary {
    public:
        const QStringList &getWords() const { return m_WordsList; }

        bool contains(const QString &word) const { return m_WordsSet.contains(word.toLower()); }

        void addWord(const QString &word) {
            QString wordToAdd = word.toLower();
            if (!m_WordsSet.contains(wordToAdd)) {
                m_WordsSet.insert(wordToAdd);
                m_WordsList.append(word);
            }
        }

        void addWords(const QStringList &words) {
            foreach (const QString &word, words) {
                addWord(word);
            }
        }

        void reset(const QStringList &words) {
            clear();
            addWords(words);
        }

        void clear() { m_WordsList.clear(); m_WordsSet.clear(); }
        bool empty() const { return m_WordsSet.isEmpty(); }
        int size() const { return m_WordsList.size(); }

    private:
        QSet<QString> m_WordsSet;
        QStringList m_WordsList;
    };

    class SpellCheckWorker : public QObject, public Common::ItemProcessingWorker<ISpellCheckItem>
    {
        Q_OBJECT

    public:
        SpellCheckWorker(Helpers::AsyncCoordinator *initCoordinator, Models::SettingsModel *settingsModel, QObject *parent=0);
        virtual ~SpellCheckWorker();

    public:
        const QStringList &getUserDictionary() const { return m_UserDictionary.getWords(); }
        QStringList retrieveCorrections(const QString &word);
        int getUserDictionarySize() const { return m_UserDictionary.size(); }

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<ISpellCheckItem> &item) override;

    private:
        void processSeparatorItem(std::shared_ptr<SpellCheckSeparatorItem> &item);
        void processQueryItem(std::shared_ptr<SpellCheckItem> &item);
        void processChangeUserDict(std::shared_ptr<ModifyUserDictItem> &item);

    protected:
        virtual void onQueueIsEmpty() override {
            /* Notify on emptiness only for batches with separator */
            /* emit queueIsEmpty(); */ }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
        void wordsNumberChanged(int number);
        void userDictUpdate(const QStringList &keywords, bool overwritten);
        void userDictCleared();

#ifdef INTEGRATION_TESTS
    public:
        int getSuggestionsCount() const { return m_Suggestions.count(); }
#endif

    private:
        void detectAffEncoding();
        QStringList suggestCorrections(const QString &word);
        bool checkWordSpelling(const std::shared_ptr<SpellCheckQueryItem> &queryItem);
        bool checkWordSpelling(const QString &word);
        void checkWordStem(const std::shared_ptr<SpellCheckQueryItem> &queryItem);
        QString getWordStem(const QString &word);
        bool isHunspellSpellingCorrect(const QString &word) const;
        void findSuggestions(const QString &word);
        void initUserDictionary();
        void cleanUserDict();
        void changeUserDict(const QStringList &words, bool overwrite);
        void signalUserDictWordsCount();

    private:
        Helpers::AsyncCoordinator *m_InitCoordinator;
        Models::SettingsModel *m_SettingsModel;
        QHash<QString, QStringList> m_Suggestions;
        QSet<QString> m_WrongWords;
        UserDictionary m_UserDictionary;
        QReadWriteLock m_SuggestionsLock;
        QString m_Encoding;
        Hunspell *m_Hunspell;
        // Coded does not need destruction
        QTextCodec *m_Codec;
        QString m_UserDictionaryPath;
    };
}

#endif // SPELLCHECKWORKER_H
