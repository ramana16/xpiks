/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadataioworker.h"
#include "../QMLExtensions/artworksupdatehub.h"
#include "../Suggestion/locallibraryquery.h"

#define METADATA_CACHE_SYNC_INTERVAL 29

namespace MetadataIO {
    MetadataIOWorker::MetadataIOWorker(Helpers::DatabaseManager *dbManager, QMLExtensions::ArtworksUpdateHub *artworksUpdateHub, QObject *parent):
        QObject(parent),
        m_MetadataCache(dbManager),
        m_ArtworksUpdateHub(artworksUpdateHub),
        m_ProcessedItemsCount(0)
    {
        Q_ASSERT(artworksUpdateHub != nullptr);
    }

    bool MetadataIOWorker::initWorker() {
        LOG_DEBUG << "#";
        bool success = m_MetadataCache.initialize();
        if (!success) {
            LOG_WARNING << "Failed to initialize metadata cache";
        }

        return true;
    }

    void MetadataIOWorker::processOneItem(std::shared_ptr<MetadataIOTaskBase> &item) {
        do {
            std::shared_ptr<MetadataReadWriteTask> readWriteItem = std::dynamic_pointer_cast<MetadataReadWriteTask>(item);
            if (readWriteItem) {
                processReadWriteItem(readWriteItem);
                break;
            }

            std::shared_ptr<MetadataCacheSyncTask> syncTask = std::dynamic_pointer_cast<MetadataCacheSyncTask>(item);
            if (syncTask) {
                m_MetadataCache.sync();
                break;
            }

            std::shared_ptr<MetadataSearchTask> searchTask = std::dynamic_pointer_cast<MetadataSearchTask>(item);
            if (searchTask) {
                processSearchItem(searchTask);
                break;
            }

            std::shared_ptr<UpdateHubHighFrequencyModeTask> updateHubTask = std::dynamic_pointer_cast<UpdateHubHighFrequencyModeTask>(item);
            if (updateHubTask) {
                break;
            }

            LOG_WARNING << "Unknown task";
            Q_ASSERT(false);
        } while(false);
    }

    void MetadataIOWorker::processReadWriteItem(std::shared_ptr<MetadataReadWriteTask> &item) {
        Models::ArtworkMetadata *artworkMetadata = item->getArtworkMetadata();
        Q_ASSERT(artworkMetadata != nullptr);
        if (artworkMetadata == nullptr) { return; }

        const MetadataReadWriteTask::ReadWriteAction action = item->getReadWriteAction();
        if (action == MetadataReadWriteTask::Read) {
            if (m_MetadataCache.read(artworkMetadata)) {
                m_ArtworksUpdateHub->updateArtwork(artworkMetadata);
            }
        } else if (action == MetadataReadWriteTask::Write) {
            m_MetadataCache.save(artworkMetadata, true);
        } else if (action == MetadataReadWriteTask::Add) {
            m_MetadataCache.save(artworkMetadata, false);
        }

        m_ProcessedItemsCount++;

        if (m_ProcessedItemsCount % METADATA_CACHE_SYNC_INTERVAL == 0) {
            m_MetadataCache.sync();
        }
    }

    void MetadataIOWorker::processSearchItem(std::shared_ptr<MetadataSearchTask> &item) {
        auto *localLibraryQuery = item->getQuery();
        m_MetadataCache.search(localLibraryQuery->getSearchQuery(), localLibraryQuery->getResults());
        localLibraryQuery->notifyResultsReady();
    }

    void MetadataIOWorker::workerStopped() {
        m_MetadataCache.finalize();
        emit stopped();
    }
}
