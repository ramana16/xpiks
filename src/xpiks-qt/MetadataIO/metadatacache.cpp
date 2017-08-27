/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadatacache.h"
#include <QFileInfo>
#include <functional>
#include "../Models/artworkmetadata.h"
#include "../Helpers/constants.h"
#include "../Common/defines.h"

namespace MetadataIO {
    CachedArtwork::CachedArtworkType queryFlagToCachedType(Common::flag_t queryFlag) {
        CachedArtwork::CachedArtworkType searchType = CachedArtwork::Unknown;

        if (Common::HasFlag(queryFlag, Suggestion::Photos)) {
            searchType = CachedArtwork::Image;
        } else if (Common::HasFlag(queryFlag, Suggestion::Vectors)) {
            searchType = CachedArtwork::Vector;
        } else if (Common::HasFlag(queryFlag, Suggestion::Videos)) {
            searchType = CachedArtwork::Video;
        }

        return searchType;
    }

    MetadataCache::MetadataCache(Helpers::DatabaseManager *dbManager):
        m_DatabaseManager(dbManager)
    {
        Q_ASSERT(dbManager != nullptr);
    }

    bool MetadataCache::initialize() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_DatabaseManager != nullptr);

        bool success = false;
        do {
            m_Database = m_DatabaseManager->openDatabase(Constants::METADATA_CACHE_DB_NAME);
            if (!m_Database) {
                LOG_WARNING << "Failed to open database";
                break;
            }

            if (!m_Database->initialize()) {
                LOG_WARNING << "Failed to initialize metadata cache";
                break;
            }

            m_DbCacheIndex = m_Database->getTable(Constants::METADATA_CACHE_TABLE);
            if (!m_DbCacheIndex) {
                LOG_WARNING << "Failed to get table" << Constants::METADATA_CACHE_TABLE;
                break;
            }

            success = true;
            LOG_INFO << "Metadata cache initialized";
        } while (false);

        return success;
    }

    void MetadataCache::finalize() {
        LOG_DEBUG << "#";

        if (m_Database) {
            m_Database->close();
        }
    }

    void MetadataCache::sync() {
        LOG_DEBUG << "#";

        flushWAL();

        if (m_Database) {
            m_Database->sync();
        }
    }

    bool MetadataCache::read(Models::ArtworkMetadata *metadata) {
        Q_ASSERT(metadata != nullptr);
        if (metadata == nullptr) { return false; }

        const QString &filepath = metadata->getFilepath();
        QByteArray rawValue;
        bool found = false;

        {
            const QByteArray key = filepath.toUtf8();

            QMutexLocker locker(&m_ReadMutex);
            Q_UNUSED(locker);

            found = m_DbCacheIndex->tryGetValue(key, rawValue);
        }

        if (found) {
            CachedArtwork value;
            QDataStream ds(&rawValue, QIODevice::ReadOnly);
            ds >> value;
            Q_ASSERT(ds.status() == QDataStream::Ok);

            if (ds.status() == QDataStream::Ok) {
                bool modified = metadata->initFromStorage(value);
                Q_UNUSED(modified);
            }
        }

        return found;
    }

    void MetadataCache::save(Models::ArtworkMetadata *metadata, bool overwrite) {
        Q_ASSERT(metadata != nullptr);
        if (metadata == nullptr) { return; }

        CachedArtwork value(metadata);
        const QString &key = metadata->getFilepath();

        if (overwrite) {
            m_SetWAL.set(key, value);
        } else {
            m_AddWal.set(key, value);
        }
    }

    void MetadataCache::search(const Suggestion::SearchQuery &query, QVector<CachedArtwork> &results) {
        CachedArtwork::CachedArtworkType searchType = queryFlagToCachedType(query.m_Flags);

        m_DbCacheIndex->foreachRow([&](QByteArray &rawKey, QByteArray &rawValue) {
            CachedArtwork value;
            QDataStream ds(&rawValue, QIODevice::ReadOnly);
            ds >> value;

            if (ds.status() != QDataStream::Ok) { /*continue;*/ return true; }
            if ((searchType != CachedArtwork::Unknown) && (value.m_ArtworkType != searchType)) { /*continue;*/ return true; }

            bool hasMatch = false;

            foreach (const QString &searchTerm, query.m_SearchTerms) {
                if (value.m_Title.contains(searchTerm, Qt::CaseInsensitive)) {
                    hasMatch = true;
                    break;
                }

                if (value.m_Description.contains(searchTerm, Qt::CaseInsensitive)) {
                    hasMatch = true;
                    break;
                }

                foreach (const QString &keyword, value.m_Keywords) {
                    if (keyword.contains(searchTerm, Qt::CaseInsensitive)) {
                        hasMatch = true;
                        break;
                    }
                }

                if (hasMatch) { break; }
            }

            if (hasMatch) {
                if (QFileInfo(QString::fromUtf8(rawKey)).exists()) {
                    results.push_back(value);
                }
            }

            const bool shouldStop = results.size() >= query.m_MaxResults;
            return shouldStop;
        });
    }

    void MetadataCache::flushWAL() {
        LOG_DEBUG << "#";
        if (!m_DbCacheIndex) { return; }

        m_AddWal.flush(m_DbCacheIndex);
        m_SetWAL.flush(m_DbCacheIndex);
    }
}
