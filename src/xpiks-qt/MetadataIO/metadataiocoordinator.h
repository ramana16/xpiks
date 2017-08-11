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
#include "../Common/baseentity.h"
#include "../Common/defines.h"

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class IMetadataReader;
    class IMetadataWriter;
    class MetadataWritingWorker;

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
        void metadataWritingFinished();
        void processingItemsCountChanged(int value);
        void discardReadingSignal();
        void hasErrorsChanged(bool value);
        void exiftoolNotFoundChanged();

    private slots:
        void readingWorkerFinished(bool success);
        void writingWorkerFinished(bool success);

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
        void readMetadataExifTool(const QVector<Models::ArtworkMetadata*> &artworksToRead, const QVector<QPair<int, int> > &rangesToUpdate);
#ifndef CORE_TESTS
        void readMetadataExiv2(const QVector<Models::ArtworkMetadata*> &artworksToRead, const QVector<QPair<int, int> > &rangesToUpdate);
#endif
        void writeMetadataExifTool(const QVector<Models::ArtworkMetadata*> &artworksToWrite, bool useBackups);
#ifndef CORE_TESTS
        void writeMetadataExiv2(const QVector<Models::ArtworkMetadata*> &artworksToWrite);
#endif
        void autoDiscoverExiftool();
        void exiftoolDiscoveryFinished();
        void tryToLaunchExiftool(const QString &settingsExiftoolPath);
        Q_INVOKABLE void discardReading();
        Q_INVOKABLE void continueReading(bool ignoreBackups);
        Q_INVOKABLE void continueWithoutReading();

    private:
        void initializeImport(int itemsCount);
        void readingFinishedHandler(bool ignoreBackups);
        void afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups);

    private:
        IMetadataReader *m_ReadingWorker;
        IMetadataWriter *m_WritingWorker;
        QString m_RecommendedExiftoolPath;
        int m_ProcessingItemsCount;
        volatile bool m_IsImportInProgress;
        volatile bool m_CanProcessResults;
        volatile bool m_IgnoreBackupsAtImport;
        volatile bool m_HasErrors;
        volatile bool m_ExiftoolNotFound;
    };
}

#endif // METADATAIOCOORDINATOR_H
