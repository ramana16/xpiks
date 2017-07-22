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

#ifndef REMOTECONFIG_H
#define REMOTECONFIG_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QJsonDocument>
#include "../Conectivity/conectivityrequest.h"
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
        friend class Conectivity::ConectivityRequest;
        void setRemoteResponse(const QByteArray &responseData);

    signals:
        void configArrived();

    private:
        QString m_ConfigUrl;
        QJsonDocument m_Config;
    };
}

#endif // REMOTECONFIG_H
