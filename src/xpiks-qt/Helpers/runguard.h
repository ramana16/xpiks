/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

namespace Helpers {
    class RunGuard
    {
    public:
        RunGuard( const QString &m_Key );
        ~RunGuard();

        bool isAnotherRunning();
        bool tryToRun();
        void release();

    private:
        const QString m_Key;
        const QString m_MemLockKey;
        const QString m_SharedMemKey;

        QSharedMemory m_SharedMem;
        QSystemSemaphore m_MemLock;

        Q_DISABLE_COPY( RunGuard )
    };
}

#endif // RUNGUARD_H
