/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
        void allBegun(int timeoutSeconds=1);
        void aboutToBegin(int operationsToAdd=1);
        void justEnded();
        void cancel();

    signals:
        void statusReported(CoordinationStatus status);

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
}

#endif // ASYNCCOORDINATOR_H
