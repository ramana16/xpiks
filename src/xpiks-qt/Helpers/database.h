/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QByteArray>
#include <QReadWriteLock>
#include <QDataStream>
#include <QAtomicInt>
#include <QMutex>
#include <QHash>
#include <QString>
#include <QVector>
#include <QPair>
#include <vector>
#include <memory>
#include <functional>
#include "asynccoordinator.h"
#include "../Common/defines.h"

struct sqlite3;
struct sqlite3_stmt;

namespace Helpers {
    template<class TValue>
    void restoreFailedItems(QHash<QString, TValue> &hash, QVector<QPair<QByteArray, QByteArray> > &keyValuesList, const QVector<int> &failedIndices) {
        for (auto &index: failedIndices) {
            auto &keyValuePair = keyValuesList[index];
            QString key = QString::fromUtf8(keyValuePair.first);

            TValue value;
            QByteArray &rawData = keyValuePair.second;
            QDataStream ds(&rawData, QIODevice::ReadOnly);
            ds >> value;

            hash.insert(key, value);
        }
    }

    // super simple wrapper over sqlite
    // to make it look like a key-value storage
    class Database {
    public:
        Database(int id, AsyncCoordinator *finalizeCoordinator);
        virtual ~Database();

    private:
        class Transaction {
        public:
            Transaction(sqlite3 *database);
            virtual ~Transaction();

        private:
            sqlite3 *m_Database;
            bool m_Started;
        };

    public:
        class Table {
        public:
            Table(sqlite3 *database, const QString &tableName);

        public:
            bool initialize();
            void finalize();

        public:
            bool tryGetValue(const QByteArray &key, QByteArray &value);
            bool trySetValue(const QByteArray &key, const QByteArray &value);
            bool tryAddValue(const QByteArray &key, const QByteArray &value);
            bool trySetMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList, QVector<int> &failedIndices);
            int tryAddMany(const QVector<QPair<QByteArray, QByteArray> > &keyValueList);
            bool tryDeleteRecord(const QByteArray &key);
            bool tryDeleteMany(const QVector<QByteArray> &keysList);
            void foreachRow(std::function<bool (QByteArray &, QByteArray &)> action);

        private:
            QString m_TableName;
            sqlite3 *m_Database;
            sqlite3_stmt *m_GetStatement;
            sqlite3_stmt *m_SetStatement;
            sqlite3_stmt *m_AddStatement;
            sqlite3_stmt *m_DelStatement;
            sqlite3_stmt *m_AllStatement;
        };

    public:
        bool open(const char *fullDbPath);
        void close();
        bool initialize();
        void finalize();
        void sync();
        std::shared_ptr<Table> getTable(const QString &name);

    private:
        void doClose();
        bool executeStatement(const char *stmt);

    private:
        int m_ID;
        AsyncCoordinator *m_FinalizeCoordinator;
        sqlite3 *m_Database;
        std::vector<std::shared_ptr<Table> > m_Tables;
        volatile bool m_IsOpened;
    };

    class DatabaseManager: public QObject {
        Q_OBJECT
    public:
        DatabaseManager();

    public:
        bool initialize();

    private:
        void finalize();
        int closeEnvironment();

    public:
        std::shared_ptr<Database> openDatabase(const QString &dbName);

    public:
        void prepareToFinalize();

    private slots:
        void onReadyToFinalize(int status);

    private:
        void closeAll();
        int getNextID();

    private:
        QMutex m_Mutex;
        AsyncCoordinator m_FinalizeCoordinator;
        QString m_DBDirPath;
        QAtomicInt m_LastDatabaseID;
        std::vector<std::shared_ptr<Database> > m_DatabaseArray;
        volatile bool m_Initialized;
    };

    template<class TKey, class TValue>
    class WriteAheadLog {
    public:
        bool tryGet(const TKey &key, TValue &value) {
            QReadLocker locker(&m_LockWAL);
            Q_UNUSED(locker);

            bool contains = false;
            if (m_WriteAheadLog.contains(key)) {
                contains = true;
                value = m_WriteAheadLog[key];
            }

            return contains;
        }

        void set(const TKey &key, const TValue &value) {
            QWriteLocker walLocker(&m_LockWAL);
            Q_UNUSED(walLocker);

            m_WriteAheadLog.insert(key, value);
        }

        void setMany(const QHash<TKey, TValue> &existing) {
            if (existing.isEmpty()) { return; }

            QWriteLocker locker(&m_LockWAL);
            Q_UNUSED(locker);

            auto it = existing.begin();
            auto itEnd = existing.end();
            while (it != itEnd) {
                m_WriteAheadLog.insert(it.key(), it.value());
                it++;
            }
        }

        void flush(std::shared_ptr<Database::Table> &dbTable) {
            LOG_DEBUG << "#";
            if (m_WriteAheadLog.empty()) { return; }

            QVector<QPair<QByteArray, QByteArray> > keyValuesList;
            {
                QWriteLocker walLocker(&m_LockWAL);
                Q_UNUSED(walLocker);

                auto it = m_WriteAheadLog.begin();
                auto itEnd = m_WriteAheadLog.end();
                for (; it != itEnd; ++it) {
                    const TKey &key = it.key();
                    const TValue &value = it.value();

                    QByteArray rawKey = keyToByteArray(key);
                    QByteArray rawValue;

                    QDataStream ds(&rawValue, QIODevice::WriteOnly);
                    ds << value;
                    Q_ASSERT(ds.status() != QDataStream::WriteFailed);

                    if (ds.status() != QDataStream::WriteFailed) {
                        keyValuesList.push_back(QPair<QByteArray, QByteArray>(rawKey, rawValue));
                    }
                }

                m_WriteAheadLog.clear();
            }

            if (keyValuesList.isEmpty()) { return; }

            QVector<int> failedIndices;
            bool success = doFlush(dbTable, keyValuesList, failedIndices);
            if (success) {
                LOG_INFO << "WAL has been flushed successfully";
            } else {
                LOG_WARNING << "Failed to flush WAL successfully. Restoring failed items...";
                QWriteLocker locker(&m_LockWAL);
                Q_UNUSED(locker);
                Helpers::restoreFailedItems<TValue>(m_WriteAheadLog, keyValuesList, failedIndices);
            }
        }

    protected:
        virtual QByteArray keyToByteArray(const TKey &key) const = 0;
        virtual bool doFlush(std::shared_ptr<Database::Table> &dbTable, const QVector<QPair<QByteArray, QByteArray> > &keyValuesList, QVector<int> &failedIndices) = 0;

    private:
        QReadWriteLock m_LockWAL;
        QHash<TKey, TValue> m_WriteAheadLog;
    };
}

#endif // DATABASE_H
