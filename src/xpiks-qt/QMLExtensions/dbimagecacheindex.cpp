/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "dbimagecacheindex.h"
#include "../Helpers/database.h"
#include "../Helpers/constants.h"

namespace QMLExtensions {
    DbImageCacheIndex::DbImageCacheIndex(Helpers::DatabaseManager *dbManager):
        DbCacheIndex(dbManager)
    {
    }

    bool DbImageCacheIndex::initialize() {
        LOG_DEBUG << "#";
        Q_ASSERT(m_DatabaseManager != nullptr);

        bool success = false;
        do {
            m_Database = m_DatabaseManager->openDatabase(Constants::IMAGECACHE_DB_NAME);
            if (!m_Database) {
                LOG_WARNING << "Failed to open database";
                break;
            }

            if (!m_Database->initialize()) {
                LOG_WARNING << "Failed to initialize images cache";
                break;
            }

            m_DbCacheIndex = m_Database->getTable(Constants::IMAGE_CACHE_TABLE);
            if (!m_DbCacheIndex) {
                LOG_WARNING << "Failed to get table" << Constants::IMAGE_CACHE_TABLE;
                break;
            }

            success = true;
            LOG_INFO << "Images cache initialized";
        } while (false);

        return success;
    }

    void DbImageCacheIndex::update(const QString &originalPath, CachedImage &cachedImage) {
        LOG_DEBUG << originalPath;
        CachedImage previous;

        if (tryGet(originalPath, previous)) {
            cachedImage.m_RequestsServed = previous.m_RequestsServed + 1;
        } else {
            cachedImage.m_RequestsServed = 1;
        }

        insert(originalPath, cachedImage);
    }

    void DbImageCacheIndex::importCache(const QHash<QString, CachedImage> &existing) {
        LOG_INFO << existing.size() << "existing item(s)";
        // TODO: add items intead of setting them in future
        m_WAL.setMany(existing);

        LOG_INFO << "Flushed" << existing.size() << "items to WAL";
    }

    int DbImageCacheIndex::getMaxCacheMemorySize() const {
#ifdef QT_DEBUG
        return 10;
#else
        return 10000;
#endif
    }
}
