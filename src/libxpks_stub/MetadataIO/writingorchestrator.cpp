/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "writingorchestrator.h"
#include <QVector>
#include <QThread>
#include <Helpers/indiceshelper.h>
#include <Models/artworkmetadata.h>
#include <Common/defines.h>
#include "metadatawritingworker.h"
#include <Helpers/asynccoordinator.h>

namespace libxpks {
    namespace io {
        WritingOrchestrator::WritingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                                 Helpers::AsyncCoordinator *asyncCoordinator,
                                                 Models::SettingsModel *settingsModel):
            m_ItemsToWriteSnapshot(artworksToWrite),
            m_SettingsModel(settingsModel),
            m_AsyncCoordinator(asyncCoordinator)
        {
            Q_ASSERT(settingsModel != nullptr);
            Q_ASSERT(asyncCoordinator != nullptr);
        }

        WritingOrchestrator::~WritingOrchestrator() {
            LOG_DEBUG << "destroyed";
        }

        void WritingOrchestrator::startWriting(bool useBackups, bool useDirectExport) {
            LOG_DEBUG << "use direct export =" << useDirectExport;

            Helpers::AsyncCoordinatorStarter deferredStarter(m_AsyncCoordinator, -1);
            Q_UNUSED(deferredStarter);

            Helpers::AsyncCoordinatorLocker locker(m_AsyncCoordinator);
            Q_UNUSED(locker);

            auto *writingWorker = new ExiftoolImageWritingWorker(m_ItemsToWriteSnapshot,
                                                                 m_AsyncCoordinator,
                                                                 m_SettingsModel,
                                                                 useBackups);
            QThread *thread = new QThread();
            writingWorker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, writingWorker, &ExiftoolImageWritingWorker::process);
            QObject::connect(writingWorker, &ExiftoolImageWritingWorker::stopped, thread, &QThread::quit);

            QObject::connect(writingWorker, &ExiftoolImageWritingWorker::stopped, writingWorker, &ExiftoolImageWritingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            thread->start();
            LOG_INFO << "Started image writing worker...";
        }

        void WritingOrchestrator::startMetadataWiping(bool useBackups) {
            Helpers::AsyncCoordinatorStarter deferredStarter(m_AsyncCoordinator, -1);
            Q_UNUSED(deferredStarter);

            Q_UNUSED(useBackups);

            LOG_INFO << "This functionality is missing from libxpks_stub";
        }
    }
}
