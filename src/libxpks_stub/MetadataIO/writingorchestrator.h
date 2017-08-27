#ifndef WRITINGORCHESTRATOR_H
#define WRITINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <MetadataIO/imetadatawriter.h>
#include <Helpers/asynccoordinator.h>
#include <MetadataIO/artworkssnapshot.h>

namespace Models {
    class ArtworkMetadata;
}

namespace libxpks {
    namespace io {
        class ExiftoolImageWritingWorker;

        class WritingOrchestrator : public QObject, public MetadataIO::IMetadataWriter
        {
            Q_OBJECT
        public:
            explicit WritingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                         Models::SettingsModel *settingsModel,
                                         QObject *parent = 0);
            virtual ~WritingOrchestrator();

        public:
            virtual const MetadataIO::ArtworksSnapshot &getArtworksSnapshot() const override { return m_ItemsToWriteSnapshot; }

        public:
            void startWriting(bool useBackups, bool useDirectExport=true);

        private:
            void startWritingImages(MetadataIO::ArtworksSnapshot::Container &rawSnapshot, bool useBackups, bool useDirectExport);
            void startWritingVideos(MetadataIO::ArtworksSnapshot::Container &rawSnapshot, bool useBackups, bool useDirectExport);

        signals:
            void allStarted();
            void allFinished(bool success);
            void cancelWriting();

        private slots:
            void onAllWorkersFinished(int status);

        private:
            ExiftoolImageWritingWorker *m_ImageWritingWorker;
            // placeholder for video writing worker
            MetadataIO::ArtworksSnapshot m_ItemsToWriteSnapshot;
            Models::SettingsModel *m_SettingsModel;
            Helpers::AsyncCoordinator m_AsyncCoordinator;
            volatile bool m_AnyError;
        };
    }
}

#endif // WRITINGORCHESTRATOR_H
