#ifndef FTPCOORDINATOR_H
#define FTPCOORDINATOR_H

#include <QObject>
#include <QVector>
#include <QSemaphore>
#include <Common/baseentity.h>
#include <Connectivity/iftpcoordinator.h>
#include <QAtomicInt>
#include <QMutex>
#include <Models/settingsmodel.h>
#include <MetadataIO/artworkssnapshot.h>

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
    class ProxySettings;
}

namespace libxpks {
    namespace net {
        class UploadContext;

        class FtpCoordinator :
                public QObject,
                public Common::BaseEntity,
                public Connectivity::IFtpCoordinator
        {
            Q_OBJECT
        public:
            explicit FtpCoordinator(int maxParallelUploads, QObject *parent = 0);

        public:
            // IFTPCOORDINATOR
            virtual void uploadArtworks(const MetadataIO::ArtworksSnapshot &artworksToUpload,
                                        std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) override;
            virtual void cancelUpload() override;

        signals:
            void uploadStarted();
            void cancelAll();
            void uploadFinished(bool anyError);
            void overallProgressChanged(double percentDone);
            void transferFailed(const QString &filepath, const QString &host);

        private slots:
            void workerProgressChanged(double oldPercents, double newPercents);
            void workerFinished(bool anyErrors);

        private:
            void initUpload(size_t uploadBatchesCount);
            void finalizeUpload();

        private:
            QMutex m_WorkerMutex;
            QSemaphore m_UploadSemaphore;
            double m_OverallProgress;
            QAtomicInt m_FinishedWorkersCount;
            volatile size_t m_AllWorkersCount;
            volatile bool m_AnyFailed;
        };
    }
}

#endif // FTPCOORDINATOR_H
