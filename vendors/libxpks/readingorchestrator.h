#ifndef READINGORCHESTRATOR_H
#define READINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QHash>
#include <MetadataIO/artworkssnapshot.h>

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class MetadataReadingHub;
}

namespace libxpks {
    namespace io {
        class ExiftoolImageReadingWorker;
        class ExiftoolVideoReadingWorker;

        class ReadingOrchestrator : public QObject
        {
            Q_OBJECT
        public:
            explicit ReadingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                         MetadataIO::MetadataReadingHub *readingHub,
                                         Models::SettingsModel *settingsModel,
                                         QObject *parent = 0);
            virtual ~ReadingOrchestrator();

        public:
            void startReading();

        private:
            void startReadingImages(MetadataIO::ArtworksSnapshot::Container &rawSnapshot);
            void startReadingVideos(MetadataIO::ArtworksSnapshot::Container &rawSnapshot);

        private:
            MetadataIO::ArtworksSnapshot m_ItemsToReadSnapshot;
            MetadataIO::MetadataReadingHub *m_ReadingHub;
            Models::SettingsModel *m_SettingsModel;
        };
    }
}

#endif // READINGORCHESTRATOR_H
