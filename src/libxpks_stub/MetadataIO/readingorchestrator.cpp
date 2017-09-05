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
#include <MetadataIO/metadatareadinghub.h>
#include "metadatareadingworker.h"

namespace libxpks {
    namespace io {
        ReadingOrchestrator::ReadingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToRead,
                                                 MetadataIO::MetadataReadingHub *readingHub,
                                                 Models::SettingsModel *settingsModel):
            m_ItemsToReadSnapshot(artworksToRead),
            m_ReadingHub(readingHub),
            m_SettingsModel(settingsModel)
        {
        }

        ReadingOrchestrator::~ReadingOrchestrator() {
        }

        void ReadingOrchestrator::startReading() {
            auto *asyncCoordinator = m_ReadingHub->getCoordinator();

            Helpers::AsyncCoordinatorStarter deferredStarter(asyncCoordinator, -1);
            Q_UNUSED(deferredStarter);

            Helpers::AsyncCoordinatorLocker locker(asyncCoordinator);
            Q_UNUSED(locker);

            ExiftoolImageReadingWorker *readingWorker = new ExiftoolImageReadingWorker(m_ItemsToReadSnapshot,
                                                                                       m_SettingsModel,
                                                                                       m_ReadingHub);
            QThread *thread = new QThread();
            readingWorker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, readingWorker, &ExiftoolImageReadingWorker::process);
            QObject::connect(readingWorker, &ExiftoolImageReadingWorker::stopped, thread, &QThread::quit);

            QObject::connect(readingWorker, &ExiftoolImageReadingWorker::stopped, readingWorker, &ExiftoolImageReadingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            LOG_DEBUG << "Starting thread...";
            thread->start();
        }
    }
}
