/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

#ifdef QT_DEBUG
    public:
        void dumpToLog();
        void dumpToArray(QVector<MetadataIO::CachedArtwork> &cachedArtworks);
        int retrieveRecordsCount();
#endif

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
