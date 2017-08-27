/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CONNECTIVITYREQUEST_H
#define CONNECTIVITYREQUEST_H

#include <QObject>
#include <QString>
#include <QByteArray>

namespace Models {
    class ProxySettings;
}

namespace Helpers {
    class RemoteConfig;
}

namespace Connectivity {
    class ConnectivityRequest: public QObject {
        Q_OBJECT

    public:
        ConnectivityRequest(Helpers::RemoteConfig *config, const QString &url, Models::ProxySettings *proxySettings):
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

#endif // CONNECTIVITYREQUEST_H
