/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "metadatareadinghub.h"
#include "metadataioservice.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"

namespace MetadataIO {
    MetadataReadingHub::MetadataReadingHub():
        m_ImportID(0),
        m_StorageReadBatchID(0),
        m_IgnoreBackupsAtImport(false),
        m_IsCancelled(false)
    {
        QObject::connect(&m_AsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                         this, &MetadataReadingHub::onCanInitialize);
    }

    void MetadataReadingHub::initializeImport(const ArtworksSnapshot &artworksToRead, int importID, quint32 storageReadBatchID) {
        m_ArtworksToRead = artworksToRead;
        m_ImportQueue.reservePush(artworksToRead.size());
        m_ImportID = importID;
        m_StorageReadBatchID = storageReadBatchID;
        m_IgnoreBackupsAtImport = false;
        m_IsCancelled = false;
        m_AsyncCoordinator.reset();
        LOG_DEBUG << "ReadingHub bound to batch ID" << m_StorageReadBatchID;
        // add 1 for the user to click a button
        m_AsyncCoordinator.aboutToBegin();
    }

    void MetadataReadingHub::finalizeImport() {
        m_ArtworksToRead.clear();
        m_ImportQueue.clear();
    }

    void MetadataReadingHub::proceedImport(bool ignoreBackups) {
        LOG_DEBUG << "ignore backups =" << ignoreBackups;
        m_IgnoreBackupsAtImport = ignoreBackups;
        m_AsyncCoordinator.justEnded();
    }

    void MetadataReadingHub::cancelImport(bool ignoreBackups) {
        LOG_DEBUG << "#";
        m_IgnoreBackupsAtImport = ignoreBackups;
        m_IsCancelled = true;
        m_AsyncCoordinator.justEnded();
    }

    void MetadataReadingHub::skipImport() {
        LOG_DEBUG << "#";
        m_ImportQueue.clear();
        m_IgnoreBackupsAtImport = true;
        m_AsyncCoordinator.justEnded();
    }

    void MetadataReadingHub::push(std::shared_ptr<OriginalMetadata> &item) {
        m_ImportQueue.push(item);
    }

    void MetadataReadingHub::onCanInitialize(int status) {
        LOG_DEBUG << "status:" << status;
        const bool ignoreBackups = m_IgnoreBackupsAtImport;
        const bool isCancelled = m_IsCancelled;

        if (ignoreBackups) {
            MetadataIOService *metadataIOService = m_CommandManager->getMetadataIOService();
            metadataIOService->cancelBatch(m_StorageReadBatchID);
        }

        const bool anyChanged = initializeArtworks(ignoreBackups, isCancelled);

        emit readingFinished(m_ImportID);

        if (anyChanged) {
            const auto &itemsToRead = m_ArtworksToRead.getWeakSnapshot();

            if (!isCancelled) {
                xpiks()->addToLibrary(itemsToRead);
            }

            xpiks()->updateArtworks(itemsToRead);
            xpiks()->submitForSpellCheck(itemsToRead);

            xpiks()->submitForWarningsCheck(itemsToRead);
        }

        finalizeImport();
    }

    bool MetadataReadingHub::initializeArtworks(bool ignoreBackups, bool isCancelled) {
        LOG_DEBUG << "ignore backups =" << ignoreBackups << "| cancelled =" << isCancelled;
        QHash<QString, size_t> filepathToIndexMap;

        std::vector<std::shared_ptr<MetadataIO::OriginalMetadata> > metadataToImport;
        // popAll() returns queue in reversed order for performance reasons
        m_ImportQueue.popAll(metadataToImport);

        if (metadataToImport.empty()) { return false; }

        bool anyChanged = false;
        const size_t size = metadataToImport.size();
        filepathToIndexMap.reserve((int)size);

        // reverse order to simulate pop() for each element
        // instead of calling popAll() above
        // more recent ones will overwrite more old ones
        // because of retrying in reading workers
        for (size_t i = 0; i < size; i++) {
            const size_t j = size - 1 - i;
            auto &originalMetadata = metadataToImport[j];
            filepathToIndexMap.insert(originalMetadata->m_FilePath, j);
        }

        const bool shouldOverwrite = ignoreBackups;
        MetadataIO::OriginalMetadata emptyOriginalMetadata;

        auto &items = m_ArtworksToRead.getRawData();
        for (auto &item: items) {
            Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
            const QString &filepath = artwork->getFilepath();

            const size_t index = filepathToIndexMap.value(filepath, size);
            if (index < size) {
                MetadataIO::OriginalMetadata *originalMetadata = metadataToImport[index].get();
                if (!isCancelled) {
                    artwork->initFromOrigin(*originalMetadata, shouldOverwrite);
                } else {
                    artwork->initFromOrigin(emptyOriginalMetadata, shouldOverwrite);
                }
            } else {
                artwork->initAsEmpty();
            }
        }

        return anyChanged;
    }
}
