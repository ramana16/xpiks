/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include "../Common/baseentity.h"

namespace Helpers {
    class AsyncCoordinator;
}

namespace Translation {
    struct DictionaryInfo {
        QString m_FullIfoPath;
        QString m_Description;
    };

    class TranslationManager :
            public QObject,
            public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool isBusy READ getIsBusy WRITE setIsBusy NOTIFY isBusyChanged)
        Q_PROPERTY(QString fullTranslation READ getFullTranslation NOTIFY fullTranslationChanged)
        Q_PROPERTY(QString shortTranslation READ getShortTranslation NOTIFY shortTranslationChanged)
        Q_PROPERTY(int selectedDictionaryIndex READ getSelectedDictionaryIndex WRITE setSelectedDictionaryIndex NOTIFY selectedDictionaryIndexChanged)
        Q_PROPERTY(bool hasMore READ getHasMore NOTIFY hasMoreChanged)
        Q_PROPERTY(QString query READ getQuery WRITE setQuery NOTIFY queryChanged)
        Q_PROPERTY(QStringList dictionaries READ getDictionariesDescriptions NOTIFY dictionariesChanged)

    public:
        explicit TranslationManager(QObject *parent = 0);

    public:
        bool getHasMore() const { return m_HasMore; }
        bool getIsBusy() const { return m_IsBusy; }
        void setIsBusy(bool value) {
            if (m_IsBusy != value) {
                m_IsBusy = value;
                emit isBusyChanged();
            }
        }

        const QString &getQuery() const { return m_Query; }
        const QString &getFullTranslation() const { return m_FullTranslation; }
        const QString &getShortTranslation() const { return m_ShortenedTranslation; }
        int getSelectedDictionaryIndex() const { return m_SelectedDictionaryIndex; }

    public:
        void initializeDictionaries(Helpers::AsyncCoordinator *initCoordinator=nullptr);
        void setQuery(const QString &value);
        void setSelectedDictionaryIndex(int value);
        QStringList getDictionariesDescriptions() const;
        void doInitializeDictionaries();
        void initializationFinished();

    private:
        bool acquireDictionary(const QString &anyDictFilePath);
        bool hasAllNeededComponents(const QString &anyDictFilePath) const;

    public:
        Q_INVOKABLE void clear();
        Q_INVOKABLE bool addDictionary(const QUrl &url);

    signals:
        void isBusyChanged();
        void fullTranslationChanged();
        void shortTranslationChanged();
        void hasMoreChanged();
        void selectedDictionaryIndexChanged();
        void queryChanged();
        void dictionariesChanged();

    public slots:
        void translationArrived();

    private slots:
        void updateTranslationTimer();

    private:
        QStringList m_AllowedSuffixes;
        QVector<DictionaryInfo> m_DictionariesList;
        QTimer m_TranslateTimer;
        QString m_DictionariesDirPath;
        QString m_Query;
        QString m_FullTranslation;
        QString m_ShortenedTranslation;
        int m_SelectedDictionaryIndex;
        volatile bool m_IsBusy;
        volatile bool m_HasMore;
    };
}

#endif // TRANSLATIONMANAGER_H
