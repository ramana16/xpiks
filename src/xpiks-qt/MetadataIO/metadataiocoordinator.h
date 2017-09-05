/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef METADATAIOCOORDINATOR_H
#define METADATAIOCOORDINATOR_H

#include <QObject>
#include <QVector>
#include <QAtomicInt>
#include "../Common/baseentity.h"
#include "../Common/defines.h"
#include "../Common/readerwriterqueue.h"
#include "originalmetadata.h"
#include "metadatareadinghub.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class MetadataWritingWorker;
    class ArtworksSnapshot;

    class MetadataIOCoordinator : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int processingItemsCount READ getProcessingItemsCount WRITE setProcessingItemsCount NOTIFY processingItemsCountChanged)
        Q_PROPERTY(bool hasErrors READ getHasErrors WRITE setHasErrors NOTIFY hasErrorsChanged)
        Q_PROPERTY(bool exiftoolNotFound READ getExiftoolNotFound WRITE setExiftoolNotFound NOTIFY exiftoolNotFoundChanged)
    public:
        MetadataIOCoordinator();

    signals:
        void metadataReadingFinished();
        void metadataReadingSkipped();
        void metadataWritingFinished();
        void processingItemsCountChanged(int value);
        void hasErrorsChanged(bool value);
        void exiftoolNotFoundChanged();

    private slots:
        void readingFinished(bool success);
        void writingFinished(bool success);

    public:
        bool getExiftoolNotFound() const { return m_ExiftoolNotFound; }
        void setExiftoolNotFound(bool value) {
            if (value != m_ExiftoolNotFound) {
                LOG_INFO << value;
                m_ExiftoolNotFound = value;
                emit exiftoolNotFoundChanged();
            }
        }

        int getProcessingItemsCount() const { return m_ProcessingItemsCount; }
        void setProcessingItemsCount(int value) {
            if (value != m_ProcessingItemsCount) {
                m_ProcessingItemsCount = value;
                emit processingItemsCountChanged(value);
            }
        }

        bool getHasErrors() const { return m_HasErrors; }
        void setHasErrors(bool value) {
            if (value != m_HasErrors) {
                m_HasErrors = value;
                emit hasErrorsChanged(value);
            }
        }

    public:
        void readMetadataExifTool(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID);
        void writeMetadataExifTool(const ArtworksSnapshot &artworksToWrite, bool useBackups);
        void autoDiscoverExiftool();
        void setRecommendedExiftoolPath(const QString &recommendedExiftool);
        Q_INVOKABLE void continueReading(bool ignoreBackups);
        Q_INVOKABLE void continueWithoutReading();

    private:
        void initializeImport(const ArtworksSnapshot &artworksToRead, quint32 storageReadBatchID);
        void readingFinishedHandler(bool ignoreBackups);
        void afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups);

    private:
        MetadataReadingHub m_ReadingHub;
        QString m_RecommendedExiftoolPath;
        volatile int m_ProcessingItemsCount;
        volatile bool m_HasErrors;
        volatile bool m_ExiftoolNotFound;
    };
}

#endif // METADATAIOCOORDINATOR_H
