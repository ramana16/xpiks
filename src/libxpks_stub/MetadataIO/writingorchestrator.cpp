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

namespace libxpks {
    namespace io {
        WritingOrchestrator::WritingOrchestrator(const MetadataIO::ArtworksSnapshot &artworksToWrite,
                                                 Models::SettingsModel *settingsModel,
                                                 QObject *parent) :
            QObject(parent),
            m_ImageWritingWorker(nullptr),
            m_ItemsToWriteSnapshot(artworksToWrite),
            m_SettingsModel(settingsModel),
            m_AnyError(false)
        {
            QObject::connect(&m_AsyncCoordinator, &Helpers::AsyncCoordinator::statusReported,
                                         this, &WritingOrchestrator::onAllWorkersFinished);
        }

        WritingOrchestrator::~WritingOrchestrator() {
            LOG_DEBUG << "destroyed";
        }

        void WritingOrchestrator::startWriting(bool useBackups, bool) {
            LOG_DEBUG << "#";

            Helpers::AsyncCoordinatorStarter deferredStarter(&m_AsyncCoordinator, -1);
            Q_UNUSED(deferredStarter);

            Helpers::AsyncCoordinatorLocker locker(&m_AsyncCoordinator);
            Q_UNUSED(locker);

            auto *writingWorker = new ExiftoolImageWritingWorker(m_ItemsToWriteSnapshot,
                                                                 &m_AsyncCoordinator,
                                                                 m_SettingsModel,
                                                                 useBackups);
            QThread *thread = new QThread();
            writingWorker->moveToThread(thread);

            QObject::connect(thread, &QThread::started, writingWorker, &ExiftoolImageWritingWorker::process);
            QObject::connect(writingWorker, &ExiftoolImageWritingWorker::stopped, thread, &QThread::quit);

            QObject::connect(writingWorker, &ExiftoolImageWritingWorker::stopped, writingWorker, &ExiftoolImageWritingWorker::deleteLater);
            QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            m_ImageWritingWorker = writingWorker;

            thread->start();
            LOG_INFO << "Started image writing worker...";

            emit allStarted();
        }

        void WritingOrchestrator::onAllWorkersFinished(int status) {
            const bool success = status == Helpers::AsyncCoordinator::AllDone;
            m_AnyError = !success || !m_ImageWritingWorker->success();
            LOG_INFO << "status:" << status << "any error:" << m_AnyError;

            m_ImageWritingWorker->dismiss();

            emit allFinished(!m_AnyError);
        }
    }
}
