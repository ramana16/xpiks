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

#ifndef METADATACACHE_H
#define METADATACACHE_H

#include <QMutex>
#include <QReadWriteLock>
#include "../Helpers/database.h"
#include "cachedartwork.h"
#include "../Suggestion/searchquery.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class ArtworkSetWAL: public Helpers::WriteAheadLog<QString, CachedArtwork> {
    protected:
        virtual QByteArray keyToByteArray(const QString &key) const override { return key.toUtf8(); }
        virtual bool doFlush(std::shared_ptr<Helpers::Database::Table> &dbTable, const QVector<QPair<QByteArray, QByteArray> > &keyValuesList, QVector<int> &failedIndices) override {
            return dbTable->trySetMany(keyValuesList, failedIndices);
        }
    };

    class ArtworkAddWAL: public Helpers::WriteAheadLog<QString, CachedArtwork> {
    protected:
        virtual QByteArray keyToByteArray(const QString &key) const override { return key.toUtf8(); }
        virtual bool doFlush(std::shared_ptr<Helpers::Database::Table> &dbTable, const QVector<QPair<QByteArray, QByteArray> > &keyValuesList, QVector<int> &failedIndices) override {
            Q_UNUSED(failedIndices);
            int count = dbTable->tryAddMany(keyValuesList);
            return count > 0;
        }
    };

    class MetadataCache
    {
    public:
        MetadataCache(Helpers::DatabaseManager *dbManager);

    public:
        bool initialize();
        void finalize();
        void sync();

    public:
        bool read(Models::ArtworkMetadata *metadata);
        void save(Models::ArtworkMetadata *metadata, bool overwrite = true);

    public:
        void search(const Suggestion::SearchQuery &query, QVector<CachedArtwork> &results);

    private:
        void flushWAL();

    private:
        QMutex m_ReadMutex;
        Helpers::DatabaseManager *m_DatabaseManager;
        std::shared_ptr<Helpers::Database::Table> m_DbCacheIndex;
        std::shared_ptr<Helpers::Database> m_Database;
        ArtworkSetWAL m_SetWAL;
        ArtworkAddWAL m_AddWal;
    };
}

#endif // METADATACACHE_H
