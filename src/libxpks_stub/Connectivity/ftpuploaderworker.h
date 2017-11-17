/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FTPUPLOADERWORKER_H
#define FTPUPLOADERWORKER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <memory>

class QSemaphore;

namespace Encryption {
    class SecretsManager;
}

namespace Models {
    class UploadInfo;
}

namespace libxpks {
    namespace net {
        class UploadBatch;

        class FtpUploaderWorker : public QObject
        {
            Q_OBJECT
        public:
            explicit FtpUploaderWorker(QSemaphore *uploadSemaphore,
                                       const std::shared_ptr<UploadBatch> &batch,
                                       const std::shared_ptr<Models::UploadInfo> &uploadInfo,
                                       QObject *parent = 0);

            virtual ~FtpUploaderWorker();

        signals:
            void uploadStarted();
            void progressChanged(double oldPercents, double newPercents);
            void uploadFinished(bool anyErrors);
            void stopped();
            void workerCancelled();
            void transferFailed(const QString &filename, const QString &host);

        public slots:
            void process();
            void progressChangedHandler(double oldPercents, double newPercents);

        private:
            void doUpload();

        private:
            QSemaphore *m_UploadSemaphore;
            std::shared_ptr<UploadBatch> m_UploadBatch;
            std::shared_ptr<Models::UploadInfo> m_UploadInfo;
            QVector<QString> m_FailedTransfers;
        };
    }
}

#endif // FTPUPLOADERWORKER_H
