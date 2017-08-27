/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REQUESTSWORKER_H
#define REQUESTSWORKER_H

#include <QObject>
#include "connectivityrequest.h"
#include "../Common/itemprocessingworker.h"

namespace Connectivity {
    class RequestsWorker: public QObject, public Common::ItemProcessingWorker<ConnectivityRequest>
    {
        Q_OBJECT
    public:
        explicit RequestsWorker(QObject *parent = 0);

    protected:
        virtual bool initWorker() override;
        virtual void processOneItem(std::shared_ptr<ConnectivityRequest> &item) override;

    protected:
        virtual void onQueueIsEmpty() override { emit queueIsEmpty(); }
        virtual void workerStopped() override { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
    };
}

#endif // REQUESTSWORKER_H
