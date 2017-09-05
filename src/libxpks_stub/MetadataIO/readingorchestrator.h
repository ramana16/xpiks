#ifndef READINGORCHESTRATOR_H
#define READINGORCHESTRATOR_H

#include <QObject>
#include <QVector>
#include <QMutex>
#include <QHash>

namespace Models {
    class ArtworkMetadata;
    class SettingsModel;
}

namespace MetadataIO {
    class MetadataReadingHub;
    class ArtworksSnapshot;
}

namespace libxpks {
    namespace io {
        class ReadingOrchestrator
        {
        public:
            explicit ReadingOrchestrator(MetadataIO::MetadataReadingHub *readingHub,
                                         Models::SettingsModel *settingsModel);
            virtual ~ReadingOrchestrator();

        public:
            void startReading();

        private:
            const MetadataIO::ArtworksSnapshot &m_ItemsToReadSnapshot;
            MetadataIO::MetadataReadingHub *m_ReadingHub;
            Models::SettingsModel *m_SettingsModel;
        };
    }
}

#endif // READINGORCHESTRATOR_H
