/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SimpleCurlRequest_H
#define SimpleCurlRequest_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QStringList>

namespace Models {
    class ProxySettings;
}

namespace Connectivity {
    class SimpleCurlRequest : public QObject
    {
        Q_OBJECT
    public:
        explicit SimpleCurlRequest(const QString &resource, bool verifySSL=false, QObject *parent = 0);

    public:
        const QByteArray &getResponseData() const { return m_ResponseData; }
        const QString &getErrorString() const { return m_ErrorString; }

    public:
        void dispose() { emit stopped(); }
        bool sendRequestSync();
        void setRawHeaders(const QStringList &headers);
        void setProxySettings(Models::ProxySettings *proxySettings);

    public slots:
        void process();

    signals:
        void requestFinished(bool success);
        void stopped();

    private:
        bool doRequest();

    private:
        QString m_RemoteResource;
        QStringList m_RawHeaders;
        QByteArray m_ResponseData;
        QString m_ErrorString;
        Models::ProxySettings *m_ProxySettings;
        bool m_VerifySSL;
    };
}

#endif // SimpleCurlRequest_H
