#ifndef WRITINGORCHESTRATOR_H
#define WRITINGORCHESTRATOR_H

#include <QVector>
#include <QMutex>
#include <MetadataIO/artworkssnapshot.h>

namespace Models {
    class ArtworkMetadata;
}

namespace Helpers {
    class AsyncCoordinator;
}

namespace libxpks {
    namespace io {
        class WritingOrchestrator
        {
        public:
            explicit WritingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                         Helpers::AsyncCoordinator *asyncCoordinator,
                                         Models::SettingsModel *settingsModel);
            virtual ~WritingOrchestrator();

        public:
            void startWriting(bool useBackups, bool useDirectExport=true);

        private:
            const MetadataIO::ArtworksSnapshot &m_ItemsToWriteSnapshot;
            Models::SettingsModel *m_SettingsModel;
            Helpers::AsyncCoordinator *m_AsyncCoordinator;
        };
    }
}


#endif // WRITINGORCHESTRATOR_H
