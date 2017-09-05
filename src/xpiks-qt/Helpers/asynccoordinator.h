/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ASYNCCOORDINATOR_H
#define ASYNCCOORDINATOR_H

#include <QObject>
#include <QAtomicInt>
#include <QTimer>
#include "../Common/iservicebase.h"

namespace Helpers {
    class AsyncCoordinator: public QObject
    {
        Q_OBJECT
    public:
        AsyncCoordinator();

    public:
        enum CoordinationStatus {
            AllDone = 0,
            Cancel,
            Timeout
        };

    public:
        void reset();
        void allBegun(int timeoutSeconds=-1);
        void aboutToBegin(int operationsToAdd=1);
        void justEnded();
        void cancel();

    signals:
        void statusReported(int status);

    private slots:
        void onTimeout();

    private:
        void reportStatus(CoordinationStatus status);

    private:
        QTimer m_Timer;
        QAtomicInt m_OpCount;
        QAtomicInt m_StatusReported;
    };

    class AsyncCoordinatorStartParams: public Common::ServiceStartParams {
    public:
        AsyncCoordinatorStartParams(AsyncCoordinator *coordinator):
            m_Coordinator(coordinator)
        {
        }

    public:
        AsyncCoordinator *m_Coordinator;
    };

    class AsyncCoordinatorLocker {
    public:
        AsyncCoordinatorLocker(AsyncCoordinator *coordinator):
            m_Coordinator(coordinator)
        {
            if (m_Coordinator != nullptr) {
                m_Coordinator->aboutToBegin();
            }
        }

        virtual ~AsyncCoordinatorLocker() {
            // if (m_Coordinator != nullptr) {
            //     m_Coordinator.justEnded();
            // }
        }

    private:
        AsyncCoordinator *m_Coordinator;
    };

    class AsyncCoordinatorUnlocker {
    public:
        AsyncCoordinatorUnlocker(AsyncCoordinator *coordinator):
            m_Coordinator(coordinator)
        {
            // if (m_Coordinator != nullptr) {
            //     m_Coordinator.aboutToBegin();
            // }
        }

        virtual ~AsyncCoordinatorUnlocker() {
            if (m_Coordinator != nullptr) {
                m_Coordinator->justEnded();
            }
        }

    private:
        AsyncCoordinator *m_Coordinator;
    };

    class AsyncCoordinatorStarter {
    public:
        AsyncCoordinatorStarter(AsyncCoordinator *coordinator, int timeout):
            m_Coordinator(coordinator),
            m_Timeout(timeout)
        {
        }

        virtual ~AsyncCoordinatorStarter() {
            if (m_Coordinator != nullptr) {
                m_Coordinator->allBegun(m_Timeout);
            }
        }

    private:
        AsyncCoordinator *m_Coordinator;
        int m_Timeout;
    };
}

#endif // ASYNCCOORDINATOR_H
