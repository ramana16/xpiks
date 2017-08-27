/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "updateservice.h"
#include "updatescheckerworker.h"
#include <QFile>
#include <QString>
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"
#include "../Common/version.h"

namespace Connectivity {
    UpdateService::UpdateService(Models::SettingsModel *settingsModel):
        m_UpdatesCheckerWorker(nullptr),
        m_SettingsModel(settingsModel),
        m_AvailableVersion(0),
        m_UpdateAvailable(false)
    {
        Q_ASSERT(settingsModel != nullptr);
    }

    void UpdateService::startChecking() {
        const bool startWorker = m_SettingsModel->getCheckForUpdates();

        if (startWorker) {
            updateSettings();
            doStartChecking();
        } else {
            LOG_INFO << "Update checking disabled";
        }
    }

    void UpdateService::stopChecking() {
        LOG_DEBUG << "#";
        emit cancelRequested();
    }

    void UpdateService::doStartChecking() {
        LOG_DEBUG << "#";

        m_UpdatesCheckerWorker = new UpdatesCheckerWorker(m_SettingsModel, m_PathToUpdate);
        QThread *thread = new QThread();
        m_UpdatesCheckerWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_UpdatesCheckerWorker, &UpdatesCheckerWorker::process);
        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped, m_UpdatesCheckerWorker, &UpdatesCheckerWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &UpdateService::cancelRequested,
                         m_UpdatesCheckerWorker, &UpdatesCheckerWorker::cancelRequested);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::updateAvailable,
                         this, &UpdateService::updateAvailable);
        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::updateDownloaded,
                         this, &UpdateService::updateDownloadedHandler);

        QObject::connect(m_UpdatesCheckerWorker, &UpdatesCheckerWorker::stopped,
                         this, &UpdateService::workerFinished);

        thread->start();
    }

    void UpdateService::updateSettings() {
        LOG_DEBUG << "#";

        int availableValue = m_SettingsModel->getAvailableUpdateVersion();

        if ((0 < availableValue) && (availableValue <= XPIKS_VERSION_INT)) {
            LOG_DEBUG << "Flushing available update settings values";
            m_SettingsModel->setPathToUpdate("");
            m_SettingsModel->setAvailableUpdateVersion(0);
            m_SettingsModel->syncronizeSettings();
        } else {
            m_PathToUpdate = m_SettingsModel->getPathToUpdate();
            m_AvailableVersion = m_SettingsModel->getAvailableUpdateVersion();
            LOG_INFO << "Available:" << m_PathToUpdate << "version:" << m_AvailableVersion;
        }
    }

    void UpdateService::workerFinished() {
        LOG_DEBUG << "#";
    }

    void UpdateService::updateDownloadedHandler(const QString &updatePath, int version) {
        LOG_DEBUG << "#";
        m_UpdateAvailable = true;
        m_PathToUpdate = updatePath;
        m_AvailableVersion = version;

        saveUpdateInfo();

        emit updateDownloaded(m_PathToUpdate);
    }

    void UpdateService::saveUpdateInfo() const {
        Q_ASSERT(m_UpdateAvailable);

        m_SettingsModel->setAvailableUpdateVersion(m_AvailableVersion);
        m_SettingsModel->setPathToUpdate(m_PathToUpdate);
    }
}
