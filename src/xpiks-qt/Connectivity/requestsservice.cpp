/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "requestsservice.h"
#include <QThread>
#include "requestsworker.h"
#include "../Helpers/remoteconfig.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

#define NO_CACHE_ATTRIBUTE true

namespace Connectivity {
    RequestsService::RequestsService(QObject *parent):
        QObject(parent),
        m_IsStopped(false)
    {
        m_RequestsWorker = new RequestsWorker();
    }

    void RequestsService::startService() {
        Q_ASSERT(!m_RequestsWorker->isRunning());
        LOG_DEBUG << "#";

        QThread *thread = new QThread();
        m_RequestsWorker->moveToThread(thread);

        QObject::connect(thread, &QThread::started, m_RequestsWorker, &RequestsWorker::process);
        QObject::connect(m_RequestsWorker, &RequestsWorker::stopped, thread, &QThread::quit);

        QObject::connect(m_RequestsWorker, &RequestsWorker::stopped, m_RequestsWorker, &RequestsWorker::deleteLater);
        QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        QObject::connect(this, &RequestsService::cancelServing,
                         m_RequestsWorker, &RequestsWorker::cancel);

        QObject::connect(m_RequestsWorker, &RequestsWorker::stopped,
                         this, &RequestsService::workerFinished);

        thread->start(/*QThread::LowPriority*/);
    }

    void RequestsService::stopService() {
        LOG_DEBUG << "#";
        m_RequestsWorker->stopWorking();
        m_IsStopped = true;
    }

    void RequestsService::receiveConfig(const QString &url, Helpers::RemoteConfig *config) {
        if (m_IsStopped) {
            LOG_DEBUG << "Skipping" << url << ". Service is stopped";
            return;
        }

        LOG_INFO << url;
        Models::ProxySettings *proxySettings = getProxySettings();

        std::shared_ptr<ConnectivityRequest> item(new ConnectivityRequest(config, url, proxySettings, NO_CACHE_ATTRIBUTE));
        m_RequestsWorker->submitItem(item);
    }

    Models::ProxySettings *RequestsService::getProxySettings() const {
        Models::SettingsModel *settings = m_CommandManager->getSettingsModel();
        Models::ProxySettings *proxySettings = settings->retrieveProxySettings();
        return proxySettings;
    }

    void RequestsService::workerFinished() {
        LOG_DEBUG << "#";
    }
}
