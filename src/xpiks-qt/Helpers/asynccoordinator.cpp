/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "asynccoordinator.h"

namespace Helpers {
    AsyncCoordinator::AsyncCoordinator():
        QObject(),
        m_OpCount(1),
        m_StatusReported(0)
    {
        m_Timer.setSingleShot(true);
        QObject::connect(&m_Timer, &QTimer::timeout, this, &AsyncCoordinator::onTimeout);
    }

    void AsyncCoordinator::reset() {
        m_OpCount.store(1);
        m_StatusReported.store(0);
    }

    void AsyncCoordinator::allBegun(int timeoutSeconds) {
        if (timeoutSeconds != -1) {
            m_Timer.start(timeoutSeconds*1000);
        }

        justEnded();
    }

    void AsyncCoordinator::aboutToBegin(int operationsToAdd) {
        m_OpCount.fetchAndAddOrdered(operationsToAdd);
    }

    void AsyncCoordinator::justEnded() {
        if (1 == m_OpCount.fetchAndSubOrdered(1)) {
            reportStatus(AllDone);
        }
    }

    void AsyncCoordinator::cancel() {
        reportStatus(Cancel);
    }

    void AsyncCoordinator::onTimeout() {
        reportStatus(Timeout);
    }

    void AsyncCoordinator::reportStatus(AsyncCoordinator::CoordinationStatus status) {
        if (0 == m_StatusReported.fetchAndStoreOrdered(1)) {
            emit statusReported((int)status);
        }
    }
}
