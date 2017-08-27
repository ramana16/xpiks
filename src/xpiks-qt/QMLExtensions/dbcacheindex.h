/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DBCACHEINDEX_H
#define DBCACHEINDEX_H

#include <QString>
#include <QHash>
#include <QReadWriteLock>
#include <QMutex>
#include <memory>
#include <functional>
#include "previewstorage.h"
#include "../Helpers/database.h"
#include "../Common/defines.h"

namespace QMLExtensions {
    template<class TValue>
    class IndexWriteAheadLog: public Helpers::WriteAheadLog<QString, TValue> {
    protected:
        virtual QByteArray keyToByteArray(const QString &key) const override { return key.toUtf8(); }
        virtual bool doFlush(std::shared_ptr<Helpers::Database::Table> &dbTable, const QVector<QPair<QByteArray, QByteArray> > &keyValuesList, QVector<int> &failedIndices) override {
            return dbTable->trySetMany(keyValuesList, failedIndices);
        }
    };

    template<class TValue>
    class DbCacheIndex: public PreviewStorage<QString, TValue>
    {
    public:
        DbCacheIndex(Helpers::DatabaseManager *dbManager):
            m_DatabaseManager(dbManager),
            m_MaxCacheTag(0)
        {
            Q_ASSERT(dbManager != nullptr);
        }

    public:
        virtual void finalize() override {
            LOG_DEBUG << "#";

            if (m_Database) {
                m_Database->close();
            }
        }

        virtual void sync() override {
            LOG_DEBUG << "#";

            flushWAL();

            if (m_Database) {
                m_Database->sync();
            }

            compactCache();
        }

        virtual bool tryGet(const QString &key, /*out*/ TValue &value) override {
            bool found = false;

            do {
                // the most fresh but not yet written to DB
                found = getFromWAL(key, value);
                if (found) { LOG_FOR_DEBUG << "Found in WAL:" << key; break; }

                // a little bit less fresh already read from DB
                found = getFromCache(key, value);
                if (found) { LOG_FOR_DEBUG << "Found in Cache:" << key; break; }

                // slow path
                found = getFromDB(key, value);
                if (found) {
                    addToCache(key, value);
                }
            } while (false);

            return found;
        }

    protected:
        virtual int getMaxCacheMemorySize() const = 0;

        void insert(const QString &key, const TValue &value) {
            LOG_FOR_DEBUG << key;

            // Fake write. Actual write to DB will be in sync()
            m_WAL.set(key, value);
        }

        bool getFromCache(const QString &key, /*out*/ TValue &value) {
            QReadLocker locker(&m_CacheLock);
            Q_UNUSED(locker);

            bool contains = false;
            if (m_CacheIndex.contains(key)) {
                contains = true;
                value = m_CacheIndex[key];
            }

            return contains;
        }

        bool getFromWAL(const QString &key, /*out*/ TValue &value) {
            return m_WAL.tryGet(key, value);
        }

        bool getFromDB(const QString &key, /*out*/ TValue &value) {
            if (!m_DbCacheIndex) { return false; }
            bool success = false;

            QByteArray rawValue;
            bool found = false;

            {
                QMutexLocker locker(&m_ReadMutex);
                Q_UNUSED(locker);

                const QByteArray utf8Key = key.toUtf8();
                found = m_DbCacheIndex->tryGetValue(utf8Key, rawValue);
            }

            if (found) {
                QDataStream ds(&rawValue, QIODevice::ReadOnly);
                ds >> value;
                Q_ASSERT(ds.status() == QDataStream::Ok);
                success = (ds.status() == QDataStream::Ok);
            }

            return success;
        }

        void addToCache(const QString &key, const TValue &value) {
            LOG_FOR_DEBUG << key;

            QWriteLocker locker(&m_CacheLock);
            Q_UNUSED(locker);
            addToCacheUnsafe(key, value);
        }

        inline void addToCacheUnsafe(const QString &key, const TValue &value) {
            m_CacheIndex.insert(key, value);

            if (value.m_RequestsServed > m_MaxCacheTag) {
                m_MaxCacheTag = value.m_RequestsServed;
            }
        }

        void flushWAL()  {
            LOG_DEBUG << "#";

            if (!m_DbCacheIndex) { return; }

            m_WAL.flush(m_DbCacheIndex);
        }

        void compactCache()  {
            LOG_DEBUG << "#";
            // if db is not available, operate only in memory
            if (!m_DbCacheIndex) { return; }

            bool needToCompact = false;
            int cacheSize = 0;
            const int maxCacheSize = getMaxCacheMemorySize();

            {
                QReadLocker locker(&m_CacheLock);
                Q_UNUSED(locker);

                cacheSize = m_CacheIndex.size();
                LOG_INFO << "Cache size is" << cacheSize << "item(s). Max size is" << maxCacheSize;
                LOG_INFO << "Max cache tag is" << m_MaxCacheTag;

                needToCompact = cacheSize >= maxCacheSize;
            }

            if (needToCompact) {
                quint64 threshold = m_MaxCacheTag / 4;
                LOG_DEBUG << "treshold:" << threshold;
                int removedCount = doCompactCache([&threshold](int, const TValue &item) -> bool {
                    return item.m_RequestsServed <= threshold;
                });

                if (removedCount <= (maxCacheSize/10)) {
                    threshold = m_MaxCacheTag / 2;
                    LOG_DEBUG << "treshold:" << threshold;
                    removedCount += doCompactCache([&threshold](int, const TValue &item) -> bool {
                        return item.m_RequestsServed <= threshold;
                    });
                }

                if (removedCount <= (maxCacheSize/10)) {
                    int eachToRemove = cacheSize / maxCacheSize;
                    LOG_DEBUG << "Removing each" << eachToRemove << "item";
                    removedCount += doCompactCache([&eachToRemove](int i, const TValue &) -> bool {
                        return (i % eachToRemove != 0);
                    });
                }
            }
        }

    private:
        int doCompactCache(const std::function<bool (int index, const TValue &item)> &pred) {
            QWriteLocker locker(&m_CacheLock);
            Q_UNUSED(locker);

            int removedCount = 0;
            int i = 0;

            auto it = m_CacheIndex.begin();
            while (it != m_CacheIndex.end()) {
                if (pred(i, it.value())) {
                    it = m_CacheIndex.erase(it);
                    removedCount++;
                } else {
                    ++it;
                }

                i++;
            }

            LOG_INFO << "Removed" << removedCount << "item(s)";

            return removedCount;
        }

    protected:
        // guard for get statement accessed from UI thread
        // and from ImageCachingWorker thread
        QMutex m_ReadMutex;
        Helpers::DatabaseManager *m_DatabaseManager;
        std::shared_ptr<Helpers::Database::Table> m_DbCacheIndex;
        std::shared_ptr<Helpers::Database> m_Database;
        IndexWriteAheadLog<TValue> m_WAL;
        QReadWriteLock m_CacheLock;
        QHash<QString, TValue> m_CacheIndex;
        quint64 m_MaxCacheTag;
    };
}

#endif // DBCACHEINDEX_H
