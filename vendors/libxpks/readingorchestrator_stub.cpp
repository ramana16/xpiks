#include <readingorchestrator.h>
#include <MetadataIO/artworkssnapshot.h>
#include <Models/artworkmetadata.h>
#include <Common/defines.h>

namespace libxpks {
    namespace io {
        ReadingOrchestrator::ReadingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                                 Models::SettingsModel *,
                                                 quint32 storageReadBatchID,
                                                 QObject *parent) :
            QObject(parent),
            m_ImageReadingWorker(nullptr),
            m_ItemsToReadSnapshot(artworksToRead),
            m_SettingsModel(nullptr),
            m_StorageReadBatchID(storageReadBatchID),
            m_AnyError(false)
        {
            LOG_DEBUG << "Orchestrator bound to batch ID" << m_StorageReadBatchID;
        }

        ReadingOrchestrator::~ReadingOrchestrator() {
            LOG_DEBUG << "destroyed with batch ID" << m_StorageReadBatchID;
        }

        void ReadingOrchestrator::startReading() {
            LOG_DEBUG << "#";

            emit allStarted();

            auto &items = m_ItemsToReadSnapshot.getRawData();
            for (auto &item: items) {
                auto *artwork = item->getArtworkMetadata();
                artwork->initAsEmpty();
            }

            emit allFinished(false);
        }

        void ReadingOrchestrator::startReadingImages(MetadataIO::ArtworksSnapshot::Container &) {
        }

        void ReadingOrchestrator::startReadingVideos(MetadataIO::ArtworksSnapshot::Container &) {
        }

        void ReadingOrchestrator::onAllWorkersFinished(int) {
        }
    }
}
