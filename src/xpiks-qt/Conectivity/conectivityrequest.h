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

#ifndef CONECTIVITYREQUEST_H
#define CONECTIVITYREQUEST_H

#include <QObject>
#include <QString>
#include <QByteArray>

namespace Models {
    class ProxySettings;
}

namespace Helpers {
    class RemoteConfig;
}

namespace Conectivity {
    class ConectivityRequest: public QObject {
        Q_OBJECT

    public:
        ConectivityRequest(Helpers::RemoteConfig *config, const QString &url, Models::ProxySettings *proxySettings):
            QObject(),
            m_RemoteConfig(config),
            m_Url(url),
            m_ProxySettings(proxySettings)
        {
            Q_ASSERT(config != nullptr);
        }

    public:
        const QString &getURL() const { return m_Url; }
        Models::ProxySettings *getProxySettings() const { return m_ProxySettings; }

    public:
        void setResponse(const QByteArray &responseData);

    signals:
        void responseReceived(bool success);

    private:
        Helpers::RemoteConfig *m_RemoteConfig;
        QString m_Url;
        Models::ProxySettings *m_ProxySettings;
    };
}

#endif // CONECTIVITYREQUEST_H
