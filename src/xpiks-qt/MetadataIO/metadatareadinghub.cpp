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
        m_InitAsEmpty(false)
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
        m_InitAsEmpty = false;
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

    void MetadataReadingHub::cancelImport() {
        LOG_DEBUG << "#";
        m_InitAsEmpty = true;
        m_AsyncCoordinator.justEnded();
    }

    void MetadataReadingHub::push(std::shared_ptr<OriginalMetadata> &item) {
        m_ImportQueue.push(item);
    }

    void MetadataReadingHub::onCanInitialize(int status) {
        LOG_DEBUG << status;
        const bool ignoreBackups = m_IgnoreBackupsAtImport;
        const bool initAsEmpty = m_InitAsEmpty;

        if (ignoreBackups) {
            MetadataIOService *metadataIOService = m_CommandManager->getMetadataIOService();
            metadataIOService->cancelBatch(m_StorageReadBatchID);
        }

        initializeArtworks(ignoreBackups, initAsEmpty);

        emit readingFinished(m_ImportID);

        const auto &itemsToRead = m_ArtworksToRead.getWeakSnapshot();

        if (!initAsEmpty) {
            xpiks()->addToLibrary(itemsToRead);
        }
        xpiks()->updateArtworks(itemsToRead);
        xpiks()->submitForSpellCheck(itemsToRead);
        xpiks()->submitForWarningsCheck(itemsToRead);

        finalizeImport();
    }

    void MetadataReadingHub::initializeArtworks(bool ignoreBackups, bool initAsEmpty) {
        LOG_DEBUG << "ignore backups =" << ignoreBackups << ", init empty =" << initAsEmpty;
        QHash<QString, size_t> filepathToIndexMap;

        std::vector<std::shared_ptr<MetadataIO::OriginalMetadata> > metadataToImport;
        // popAll() returns queue in reversed order for performance reasons
        m_ImportQueue.popAll(metadataToImport);

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

        auto &items = m_ArtworksToRead.getRawData();
        for (auto &item: items) {
            Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
            const QString &filepath = artwork->getFilepath();

            const size_t index = filepathToIndexMap.value(filepath, size);
            if (index < size) {
                MetadataIO::OriginalMetadata *originalMetadata = metadataToImport[index].get();
                if (!initAsEmpty) {
                    artwork->initFromOrigin(*originalMetadata, shouldOverwrite);
                } else {
                    artwork->initAsEmpty(*originalMetadata);
                }
            } else {
                artwork->initAsEmpty();
            }
        }
    }
}
