/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FTPCOORDINATOR_H
#define FTPCOORDINATOR_H

#include <QObject>
#include <QVector>
#include <QSemaphore>
#include "../Common/baseentity.h"
#include "iftpcoordinator.h"
#include <QAtomicInt>
#include <QMutex>
#include "../Models/settingsmodel.h"

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
    class ProxySettings;
}

namespace Conectivity {
    class UploadContext;

    class FtpCoordinator :
            public QObject,
            public Common::BaseEntity,
            public IFtpCoordinator
    {
        Q_OBJECT
    public:
        explicit FtpCoordinator(int maxParallelUploads, QObject *parent = 0);

    public:
        // IFTPCOORDINATOR
        virtual void uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
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

#endif // FTPCOORDINATOR_H
