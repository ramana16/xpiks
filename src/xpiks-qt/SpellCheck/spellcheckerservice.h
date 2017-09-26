/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SPELLCHECKERSERVICE_H
#define SPELLCHECKERSERVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include "../Common/basickeywordsmodel.h"
#include "../Common/iservicebase.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"

namespace Models {
    class ArtworkMetadata;
}

namespace SpellCheck {
    class SpellCheckWorker;

    class SpellCheckerService:
        public QObject,
        public Common::IServiceBase<Common::BasicKeywordsModel, Common::SpellCheckFlags>
    {
    Q_OBJECT
    Q_PROPERTY(int userDictWordsNumber READ getUserDictWordsNumber NOTIFY userDictWordsNumberChanged)

    public:
        SpellCheckerService(Models::SettingsModel *settingsModel = 0);

        virtual ~SpellCheckerService();

    public:
        virtual void startService(const std::shared_ptr<Common::ServiceStartParams> &params) override;
        virtual void stopService() override;

        virtual bool isAvailable() const override { return true; }
        virtual bool isBusy() const override;

        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck) override;
        virtual void submitItem(Common::BasicKeywordsModel *itemToCheck, Common::SpellCheckFlags flags) override;
        virtual void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck) override;
        void submitItems(const QVector<Common::BasicKeywordsModel *> &itemsToCheck, const QStringList &wordsToCheck);
        void submitKeyword(Common::BasicKeywordsModel *itemToCheck, int keywordIndex);
        virtual QStringList suggestCorrections(const QString &word) const;
        void restartWorker();
        int getUserDictWordsNumber();

#ifdef INTEGRATION_TESTS
    public:
        int getSuggestionsCount();
#endif

    public:
        QStringList getUserDictionary() const;
        void updateUserDictionary(const QStringList &words);

    public:
        Q_INVOKABLE void cancelCurrentBatch();
        Q_INVOKABLE bool hasAnyPending();
        Q_INVOKABLE void addWordToUserDictionary(const QString &word);
        Q_INVOKABLE void clearUserDictionary();

    signals:
        void cancelSpellChecking();
        void spellCheckQueueIsEmpty();
        void serviceAvailable(bool afterRestart);
        void userDictWordsNumberChanged();
        void userDictUpdate(const QStringList &keywords, bool overwritten);
        void userDictCleared();

    private slots:
        void workerFinished();
        void workerDestroyed(QObject *object);
        void wordsNumberChangedHandler(int number);

    private:
        Common::WordAnalysisFlags getWordAnalysisFlags() const;

    private:
        SpellCheckWorker *m_SpellCheckWorker;
        Models::SettingsModel *m_SettingsModel;
        volatile bool m_RestartRequired;
        QString m_DictionariesPath;
    };
}

#endif // SPELLCHECKERSERVICE_H
