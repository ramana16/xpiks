/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "requestsservice.h"
#include <QThread>
#include "requestsworker.h"
#include "../Helpers/remoteconfig.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

namespace Conectivity {
    RequestsService::RequestsService(QObject *parent):
        QObject(parent)
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
    }

    void RequestsService::receiveConfig(const QString &url, Helpers::RemoteConfig *config) {
        LOG_INFO << url;
        Models::ProxySettings *proxySettings = getProxySettings();

        std::shared_ptr<ConectivityRequest> item(new ConectivityRequest(config, url, proxySettings));
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
