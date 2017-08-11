/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ftpuploaderworker.h"
#include <QSemaphore>
#include <QCoreApplication>
#include "curlftpuploader.h"
#include "../Models/uploadinfo.h"
#include "../Common/defines.h"
#include "uploadbatch.h"

namespace Conectivity {
    FtpUploaderWorker::FtpUploaderWorker(QSemaphore *uploadSemaphore,
                                         const std::shared_ptr<UploadBatch> &batch,
                                         const std::shared_ptr<Models::UploadInfo> &uploadInfo,
                                         QObject *parent) :
        QObject(parent),
        m_UploadSemaphore(uploadSemaphore),
        m_UploadBatch(batch),
        m_UploadInfo(uploadInfo)
    {
    }

    FtpUploaderWorker::~FtpUploaderWorker() {
    }

    void FtpUploaderWorker::process() {
        const QString &host = m_UploadBatch->getContext()->m_Host;

        LOG_DEBUG << "Waiting for the semaphore" << host;
        m_UploadSemaphore->acquire();

        LOG_INFO << "Starting upload to" << host;
        doUpload();

        m_UploadSemaphore->release();
        LOG_DEBUG << "Released semaphore" << host;

        emit stopped();
    }

    void FtpUploaderWorker::progressChangedHandler(double oldPercents, double newPercents) {
        Q_UNUSED(oldPercents);
        m_UploadInfo->setPercent(floor(newPercents));
    }

    void FtpUploaderWorker::doUpload() {
        CurlFtpUploader ftpUploader(m_UploadBatch);

        //QObject::connect(&ftpUploader, SIGNAL(uploadStarted()), this, SIGNAL(uploadStarted()));
        QObject::connect(&ftpUploader, &CurlFtpUploader::uploadFinished, this, &FtpUploaderWorker::uploadFinished);
        QObject::connect(&ftpUploader, &CurlFtpUploader::progressChanged, this, &FtpUploaderWorker::progressChanged);
        QObject::connect(&ftpUploader, &CurlFtpUploader::progressChanged, this, &FtpUploaderWorker::progressChangedHandler);
        QObject::connect(this, &FtpUploaderWorker::workerCancelled, &ftpUploader, &CurlFtpUploader::cancel);
        QObject::connect(&ftpUploader, &CurlFtpUploader::transferFailed, this, &FtpUploaderWorker::transferFailed);

        ftpUploader.uploadBatch();
        // in order to deliver 100% progressChanged() signal
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

