/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "remoteconfig.h"
#include <QThread>
#include "../Common/defines.h"
#include "../Conectivity/simplecurlrequest.h"
#include "../Models/proxysettings.h"

namespace Helpers {
    RemoteConfig::RemoteConfig(QObject *parent):
        QObject(parent)
    {
    }

    RemoteConfig::~RemoteConfig() {
    }

    void RemoteConfig::setRemoteResponse(const QByteArray &responseData) {
        QJsonParseError error;
        LOG_INTEGR_TESTS_OR_DEBUG << responseData;

        m_Config = QJsonDocument::fromJson(responseData, &error);

        if (error.error == QJsonParseError::NoError) {
            emit configArrived();
        } else {
            LOG_INTEGRATION_TESTS << m_ConfigUrl;
            LOG_WARNING << "Failed to parse remote json" << error.errorString();
        }
    }
}
