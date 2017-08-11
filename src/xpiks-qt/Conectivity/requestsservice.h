/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REQUESTSSERVICE_H
#define REQUESTSSERVICE_H

#include <QObject>
#include "../Common/baseentity.h"

namespace Helpers {
    class RemoteConfig;
}

namespace Models {
    class ProxySettings;
}

namespace Conectivity {
    class RequestsWorker;

    class RequestsService : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        explicit RequestsService(QObject *parent = 0);

    public:
        void startService();
        void stopService();

    public:
        void receiveConfig(const QString &url, Helpers::RemoteConfig *config);

    private:
        Models::ProxySettings *getProxySettings() const;

    signals:
        void cancelServing();

    private slots:
        void workerFinished();

    private:
        RequestsWorker *m_RequestsWorker;
    };
}

#endif // REQUESTSSERVICE_H
