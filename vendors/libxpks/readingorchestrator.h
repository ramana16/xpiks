#ifndef READINGORCHESTRATOR_H
#define READINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QHash>
#include <MetadataIO/imetadatareader.h>
#include <Helpers/asynccoordinator.h>
#include <MetadataIO/artworkssnapshot.h>

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace libxpks {
    namespace io {
        class ExiftoolImageReadingWorker;

        class ReadingOrchestrator : public QObject, public MetadataIO::IMetadataReader
        {
            Q_OBJECT
        public:
            explicit ReadingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                         Models::SettingsModel *settingsModel,
                                         quint32 storageReadBatchID,
                                         QObject *parent = 0);
            virtual ~ReadingOrchestrator();

        public:
            virtual const QVector<QHash<QString, MetadataIO::OriginalMetadata> > &getImportResults() const override { return m_ImportResults; }
            virtual const MetadataIO::ArtworksSnapshot &getArtworksSnapshot() const override { return m_ItemsToReadSnapshot; }
            virtual quint32 getReadingBatchID() const override { return m_StorageReadBatchID; }

        public:
            void startReading();

        private:
            void startReadingImages(MetadataIO::ArtworksSnapshot::Container &rawSnapshot);
            void startReadingVideos(MetadataIO::ArtworksSnapshot::Container &rawSnapshot);

        signals:
            void allStarted();
            void allFinished(bool success);
            void cancelReading();

        private slots:
            void onAllWorkersFinished(int status);

        private:
            ExiftoolImageReadingWorker *m_ImageReadingWorker;
            // placeholder for video reading worker
            MetadataIO::ArtworksSnapshot m_ItemsToReadSnapshot;
            QVector<QHash<QString, MetadataIO::OriginalMetadata> > m_ImportResults;
            Models::SettingsModel *m_SettingsModel;
            Helpers::AsyncCoordinator m_AsyncCoordinator;
            quint32 m_StorageReadBatchID;
            volatile bool m_AnyError;
        };
    }
}

#endif // READINGORCHESTRATOR_H
