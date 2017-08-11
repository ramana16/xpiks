/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TELEMETRYSERVICE_H
#define TELEMETRYSERVICE_H

#include <QObject>
#include <QString>
#include "analyticsuserevent.h"

namespace Conectivity {
    class TelemetryWorker;

    class TelemetryService : public QObject {
        Q_OBJECT
    public:
        TelemetryService(const QString &userId, bool telemetryEnabled, QObject *parent=NULL);

    public:
        void startReporting();
        void stopReporting(bool immediately=true);

    private:
        void doStartReporting();

    public:
        void reportAction(UserAction action);
        void setEndpoint(const QString &endpoint);
        void setInterfaceLanguage(const QString &language) { m_InterfaceLanguage = language; }

    public slots:
        void changeReporting(bool value);

    private slots:
        void workerDestroyed(QObject* object);

    signals:
        void cancelAllQueries();

    private:
        TelemetryWorker *m_TelemetryWorker;
        QString m_ReportingEndpoint;
        QString m_UserAgentId;
        QString m_InterfaceLanguage;
        volatile bool m_TelemetryEnabled;
        volatile bool m_RestartRequired;
    };
}

#endif // TELEMETRYSERVICE_H

