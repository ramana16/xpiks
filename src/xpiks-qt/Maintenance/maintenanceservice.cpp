/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QThread>
#include "../Common/defines.h"
#include "maintenanceservice.h"
#include "maintenanceworker.h"
#include "logscleanupjobitem.h"
#include "updatescleanupjobitem.h"
#include "launchexiftooljobitem.h"
#include "initializedictionariesjobitem.h"
#include "movesettingsjobitem.h"
#include "savesessionjobitem.h"
#include "moveimagecachejobitem.h"
#include "xpkscleanupjob.h"

namespace Maintenance {
    MaintenanceService::MaintenanceService():
        m_MaintenanceWorker(NULL)
    {
    }

    void MaintenanceService::startService() {
        LOG_DEBUG << "#";

        if (m_MaintenanceWorker != NULL) {
            LOG_WARNING << "Attempt to start running worker";
            return;
        }

        m_MaintenanceWorker = new MaintenanceWorker();

        QThread *thread = new QThread();
        m_MaintenanceWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_MaintenanceWorker, &MaintenanceWorker::process);
        QObject::connect(m_MaintenanceWorker, &MaintenanceWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_MaintenanceWorker, &MaintenanceWorker::stopped, m_MaintenanceWorker, &MaintenanceWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(m_MaintenanceWorker, &MaintenanceWorker::stopped,
                         this, &MaintenanceService::workerFinished);
        QObject::connect(m_MaintenanceWorker, &MaintenanceWorker::destroyed,
                         this, &MaintenanceService::workerDestroyed);

        LOG_DEBUG << "starting low priority thread...";
        thread->start(QThread::LowPriority);
    }

    void MaintenanceService::stopService() {
        LOG_DEBUG << "#";
        if (m_MaintenanceWorker != NULL) {
            m_MaintenanceWorker->stopWorking();
        } else {
            LOG_WARNING << "MaintenanceWorker is NULL";
        }
    }

    void MaintenanceService::cleanupUpdatesArtifacts() {
#ifdef Q_OS_WIN
        LOG_DEBUG << "#";
        std::shared_ptr<IMaintenanceItem> jobItem(new UpdatesCleanupJobItem());
        m_MaintenanceWorker->submitItem(jobItem);
#endif
    }

    void MaintenanceService::launchExiftool(const QString &settingsExiftoolPath, MetadataIO::MetadataIOCoordinator *coordinator) {
        LOG_DEBUG << "#";
        std::shared_ptr<IMaintenanceItem> jobItem(new LaunchExiftoolJobItem(settingsExiftoolPath, coordinator));
        m_MaintenanceWorker->submitItem(jobItem);
    }

    void MaintenanceService::initializeDictionaries(Translation::TranslationManager *translationManager, Helpers::AsyncCoordinator *initCoordinator) {
        LOG_DEBUG << "#";
        std::shared_ptr<IMaintenanceItem> jobItem(new InitializeDictionariesJobItem(translationManager, initCoordinator));
        m_MaintenanceWorker->submitFirst(jobItem);
    }

    void MaintenanceService::cleanupLogs() {
#ifdef WITH_LOGS
        LOG_DEBUG << "#";
        std::shared_ptr<IMaintenanceItem> jobItem(new LogsCleanupJobItem());
        m_MaintenanceWorker->submitItem(jobItem);
#endif
    }

    void MaintenanceService::moveSettings(Models::SettingsModel *settingsModel) {
        std::shared_ptr<IMaintenanceItem> jobItem(new MoveSettingsJobItem(settingsModel));
        m_MaintenanceWorker->submitFirst(jobItem);
    }

    void MaintenanceService::upgradeImagesCache(QMLExtensions::ImageCachingService *imageCachingService) {
        std::shared_ptr<IMaintenanceItem> jobItem(new MoveImageCacheJobItem(imageCachingService));
        m_MaintenanceWorker->submitItem(jobItem);
    }

    void MaintenanceService::saveSession(std::unique_ptr<MetadataIO::SessionSnapshot> &sessionSnapshot, Models::SessionManager *sessionManager) {
        LOG_DEBUG << "#";

        std::shared_ptr<IMaintenanceItem> jobItem(new SaveSessionJobItem(sessionSnapshot, sessionManager));
        m_MaintenanceWorker->submitItem(jobItem);
    }

    void MaintenanceService::cleanupOldXpksBackups(const QString &directory) {
        LOG_DEBUG << directory;

        std::shared_ptr<IMaintenanceItem> jobItem(new XpksCleanupJob(directory));
        m_MaintenanceWorker->submitItem(jobItem);
    }

    void MaintenanceService::workerFinished() {
        LOG_DEBUG << "#";
    }

    void MaintenanceService::workerDestroyed(QObject *object) {
        Q_UNUSED(object);
        LOG_DEBUG << "#";
        m_MaintenanceWorker = NULL;
    }
}
