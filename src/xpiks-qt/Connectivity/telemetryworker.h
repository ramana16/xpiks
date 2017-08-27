/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TELEMETRYWORKER_H
#define TELEMETRYWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "analyticsuserevent.h"

class QNetworkReply;

namespace Connectivity {
    class TelemetryWorker : public QObject, public Common::ItemProcessingWorker<AnalyticsUserEvent>
    {
        Q_OBJECT
    public:
        TelemetryWorker(const QString &userAgent, const QString &reportingEndpoint, const QString &interfaceLanguage);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<AnalyticsUserEvent> &item) override;

    private:
        bool sendOneReport(const QString &resource, const QString &payload);

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
        void cancelAllQueries();

    private:
        QString m_UserAgentId;
        QString m_ReportingEndpoint;
        QString m_InterfaceLanguage;
    };
}

#endif // TELEMETRYWORKER_H
