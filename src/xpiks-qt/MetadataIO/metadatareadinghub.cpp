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
        // m_ArtworksToRead.set(artworksToRead);
        m_ImportQueue.reservePush(artworksToRead);
        m_StorageReadBatchID = storageReadBatchID;
        m_IgnoreBackupsAtImport = false;
        m_InitAsEmpty = false;
        LOG_DEBUG << "ReadingHub bound to batch ID" << m_StorageReadBatchID;
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
        // todo:
        if (m_IgnoreBackupsAtImport) {
            quint32 batchID = m_ReadingWorker->getReadingBatchID();
            MetadataIOService *metadataIOService = m_CommandManager->getMetadataIOService();
            metadataIOService->cancelBatch(batchID);
        }
    }

    void MetadataReadingHub::initializeArtworks() {
        QHash<QString, size_t> filepathToIndexMap;

        std::vector<std::shared_ptr<MetadataIO::OriginalMetadata> > metadataToImport;
        m_ImportQueue.popAll(metadataToImport);

        const size_t size = metadataToImport.size();
        filepathToIndexMap.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            auto &originalMetadata = metadataToImport[i];
            filepathToIndexMap.insert(originalMetadata->m_FilePath, i);
        }

        const bool shouldOverwrite = m_IgnoreBackupsAtImport;
        const bool initAsEmpty = m_InitAsEmpty;

        auto &items = m_ArtworksToRead.getRawData();
        for (auto &item: items) {
            Models::ArtworkMetadata *artwork = item->getArtworkMetadata();
            const QString &filepath = artwork->getFilepath();

            size_t index = filepathToIndexMap.value(filepath, size);
            if (index < size) {
                MetadataIO::OriginalMetadata *originalMetadata = metadataToImport[index].get();
                metadata->initFromOrigin(*originalMetadata, shouldOverwrite);
            }
        }
    }

    void MetadataReadingHub::initEmpty() {
        MetadataIOService *metadataIOService = m_CommandManager->getMetadataIOService();
        metadataIOService->cancelBatch(m_StorageReadBatchID);

        auto &items = m_ArtworksToRead.getRawData();

        for (const auto &item: items) {
            Models::ArtworkMetadata *metadata = item->getArtworkMetadata();
            const QString &filepath = metadata->getFilepath();

            bool found = false;
            for (auto &importResult: importResults) {
                if (importResult.contains(filepath)) {
                    const OriginalMetadata &importResultItem = importResult.value(filepath);
                    metadata->initAsEmpty(importResultItem);
                    found = true;
                    break;
                }
            }

            if (!found) {
                metadata->initAsEmpty();
            }
        }
    }
}
