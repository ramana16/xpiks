#include "ftpcoordinator.h"
#include <Common/defines.h>

namespace libxpks {
    namespace net {
        FtpCoordinator::FtpCoordinator(int maxParallelUploads, QObject *parent) :
            QObject(parent),
            m_UploadSemaphore(maxParallelUploads),
            m_OverallProgress(0.0),
            m_FinishedWorkersCount(0),
            m_AllWorkersCount(0),
            m_AnyFailed(false)
        {
        }

        void FtpCoordinator::uploadArtworks(const MetadataIO::ArtworksSnapshot &,
                                            std::vector<std::shared_ptr<Models::UploadInfo> > &) {
            emit uploadFinished(true);
            emit overallProgressChanged(100.0);
        }

        void FtpCoordinator::cancelUpload() {
            emit cancelAll();
        }

        void FtpCoordinator::workerProgressChanged(double, double) {
        }

        void FtpCoordinator::workerFinished(bool) {
        }

        void FtpCoordinator::initUpload(size_t) {
        }

        void FtpCoordinator::finalizeUpload() {
        }
    }
}
