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
        m_StorageReadBatchID(0),
        m_IgnoreBackupsAtImport(false),
        m_InitAsEmpty(false)
    {
        QObject::connect(&m_AsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                         this, &MetadataReadingHub::onCanInitialize);
    }

    void MetadataReadingHub::initializeImport(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID) {
        m_ArtworksToRead = artworksToRead;
        m_ImportQueue.reservePush(artworksToRead.size());
        m_StorageReadBatchID = storageReadBatchID;
        m_IgnoreBackupsAtImport = false;
        m_InitAsEmpty = false;
        m_AsyncCoordinator.reset();
        LOG_DEBUG << "ReadingHub bound to batch ID" << m_StorageReadBatchID;
    }

    void MetadataReadingHub::finalizeImport() {
        m_ArtworksToRead.clear();
        m_ImportQueue.clear();
    }

    void MetadataReadingHub::proceedImport(bool ignoreBackups) {
        m_IgnoreBackupsAtImport = ignoreBackups;
        m_AsyncCoordinator.justEnded();
    }

    void MetadataReadingHub::cancelImport() {
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

        emit readingFinished();

        const auto &itemsToRead = m_ArtworksToRead.getWeakSnapshot();

        if (!initAsEmpty) {
            m_CommandManager->addToLibrary(itemsToRead);
        }
        m_CommandManager->updateArtworks(itemsToRead);
        m_CommandManager->submitForSpellCheck(itemsToRead);
        m_CommandManager->submitForWarningsCheck(itemsToRead);

        finalizeImport();
    }

    void MetadataReadingHub::initializeArtworks(bool ignoreBackups, bool initAsEmpty) {
        QHash<QString, size_t> filepathToIndexMap;

        std::vector<std::shared_ptr<MetadataIO::OriginalMetadata> > metadataToImport;
        m_ImportQueue.popAll(metadataToImport);

        const size_t size = metadataToImport.size();
        filepathToIndexMap.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            auto &originalMetadata = metadataToImport[i];
            filepathToIndexMap.insert(originalMetadata->m_FilePath, i);
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
