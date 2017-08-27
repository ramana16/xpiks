#include <writingorchestrator.h>
#include <MetadataIO/artworkssnapshot.h>
#include <Models/artworkmetadata.h>
#include <Common/defines.h>

namespace libxpks {
    namespace io {
        WritingOrchestrator::WritingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                                 Models::SettingsModel *settingsModel,
                                                 QObject *parent):
            QObject(parent),
            m_ImageWritingWorker(nullptr),
            m_ItemsToWriteSnapshot(artworksToWrite),
            m_SettingsModel(nullptr),
            m_AnyError(false)
        {
        }

        WritingOrchestrator::~WritingOrchestrator() {
            LOG_DEBUG << "destroyed";
        }

        void WritingOrchestrator::startWriting(bool, bool) {
            LOG_DEBUG << "#";

            emit allStarted();

            auto &items = m_ItemsToWriteSnapshot.getRawData();
            for (auto &item: items) {
                auto *artwork = item->getArtworkMetadata();
                artwork->resetModified();
            }

            emit allFinished(false);
        }

        void WritingOrchestrator::startWritingImages(MetadataIO::ArtworksSnapshot::Container &, bool , bool) {
        }

        void WritingOrchestrator::startWritingVideos(MetadataIO::ArtworksSnapshot::Container &, bool , bool ) {
        }

        void WritingOrchestrator::onAllWorkersFinished(int ) {
        }
    }
}
