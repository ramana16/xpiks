/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOTECONFIG_H
#define REMOTECONFIG_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include "../Connectivity/connectivityrequest.h"
#include "../Common/defines.h"

namespace Models {
    class ProxySettings;
}

namespace Helpers {
    class RemoteConfig : public QObject {
        Q_OBJECT
    public:
        RemoteConfig(QObject *parent=0);
        virtual ~RemoteConfig();

    public:
        void setConfigUrl(const QString &configUrl) { m_ConfigUrl = configUrl; }
        const QJsonDocument& getConfig() const { return m_Config; }

    private:
        friend class Connectivity::ConnectivityRequest;
        void setRemoteResponse(const QByteArray &responseData);

    signals:
        void configArrived();

    private:
        QString m_ConfigUrl;
        QJsonDocument m_Config;
    };
}

#endif // REMOTECONFIG_H
