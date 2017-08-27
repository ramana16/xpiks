/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "abstractconfigupdatermodel.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Connectivity/requestsservice.h"

namespace Models {
    AbstractConfigUpdaterModel::AbstractConfigUpdaterModel(bool forceOverwrite, QObject *parent):
        QObject(parent),
        m_RemoteConfig(this),
        m_ForceOverwrite(forceOverwrite)
    {
        QObject::connect(&m_RemoteConfig, &Helpers::RemoteConfig::configArrived, this, &AbstractConfigUpdaterModel::remoteConfigArrived);
    }

    void AbstractConfigUpdaterModel::initializeConfigs(const QString &configUrl, const QString &filePath) {
        LOG_DEBUG << "#";
        initLocalConfig(filePath);
        initRemoteConfig(configUrl);
    }

    void AbstractConfigUpdaterModel::remoteConfigArrived() {
        LOG_DEBUG << "#";
        const QJsonDocument &remoteDocument = m_RemoteConfig.getConfig();
        processRemoteConfig(remoteDocument, m_ForceOverwrite);
    }

    void AbstractConfigUpdaterModel::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        LOG_DEBUG << "#";
        QJsonDocument &localDocument = m_LocalConfig.getConfig();
        Helpers::mergeJson(remoteDocument, localDocument, overwriteLocal, *this);
        m_LocalConfig.saveToFile();
    }

    void AbstractConfigUpdaterModel::initRemoteConfig(const QString &configUrl) {
        Q_ASSERT(m_CommandManager != NULL);
        Q_ASSERT(!configUrl.isEmpty());
        m_RemoteConfig.setConfigUrl(configUrl);

        Connectivity::RequestsService *requestsService = m_CommandManager->getRequestsService();
        requestsService->receiveConfig(configUrl, &m_RemoteConfig);
    }

    void AbstractConfigUpdaterModel::initLocalConfig(const QString &filePath){
        m_LocalConfig.initConfig(filePath);
        const QJsonDocument &localDocument = m_LocalConfig.getConfig();
        processLocalConfig(localDocument);
    }
}
