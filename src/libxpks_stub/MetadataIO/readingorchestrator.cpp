/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "readingorchestrator.h"
#include <QThread>
#include <QVector>
#include <QMutexLocker>
#include <Models/artworkmetadata.h>
#include <Common/defines.h>
#include "metadatareadingworker.h"

namespace libxpks {
    namespace io {
        ReadingOrchestrator::ReadingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                                 Models::SettingsModel *settingsModel,
                                                 quint32 storageReadBatchID,
                                                 QObject *parent) :
            QObject(parent),
            m_ImageReadingWorker(nullptr),
            m_ItemsToReadSnapshot(artworksToRead),
            m_SettingsModel(settingsModel),
            m_StorageReadBatchID(storageReadBatchID),
            m_AnyError(false)

        {
            QObject::connect(&m_AsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                                         this, &ReadingOrchestrator::onAllWorkersFinished);
        }

        ReadingOrchestrator::~ReadingOrchestrator() {
            LOG_DEBUG << "destroyed";
        }

        void ReadingOrchestrator::startReading() {
            Helpers::AsyncCoordinatorStarter deferredStarter(&m_AsyncCoordinator, -1);
            Q_UNUSED(deferredStarter);

            Helpers::AsyncCoordinatorLocker locker(&m_AsyncCoordinator);
            Q_UNUSED(locker);

            ExiftoolImageReadingWorker *readingWorker = new ExiftoolImageReadingWorker(m_ItemsToReadSnapshot,
                                                                                       m_SettingsModel,
                                                                                       &m_AsyncCoordinator);
            QThread *thread = new QThread();
            readingWorker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, readingWorker, &ExiftoolImageReadingWorker::process);
            QObject::connect(readingWorker, &ExiftoolImageReadingWorker::stopped, thread, &QThread::quit);

            QObject::connect(readingWorker, &ExiftoolImageReadingWorker::stopped, readingWorker, &ExiftoolImageReadingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            LOG_DEBUG << "Starting thread...";
            thread->start();

            m_ImageReadingWorker = readingWorker;

            emit allStarted();
        }

        void ReadingOrchestrator::onAllWorkersFinished(int status) {
            const bool success = status == Helpers::AsyncCoordinator::AllDone;
            m_AnyError = !success || !m_ImageReadingWorker->success();
            LOG_INFO << "status:" << status << "any error:" << m_AnyError;

            m_ImportResults.append(m_ImageReadingWorker->getImportResult());

            m_ImageReadingWorker->dismiss();

            emit allFinished(!m_AnyError);
        }
    }
}
