/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef THREADHELPERS_H
#define THREADHELPERS_H

#include <QWaitCondition>
#include <QMutex>
#include <QSemaphore>

namespace Helpers {
    class ManualResetEvent
    {
    public:
        ManualResetEvent();

    public:
        void set();
        void waitOne();

    private:
        QWaitCondition m_WaitCondition;
        QMutex m_Mutex;
        volatile bool m_Flag;
    };

    class Barrier {
    public:
        Barrier(int threadsNumber);

    public:
        void wait();

    private:
        void phase1();
        void phase2();

    private:
        QMutex m_Mutex;
        QSemaphore m_Turnstile;
        QSemaphore m_Turnstile2;
        volatile int m_ThreadsNumber;
        volatile int m_Count;
    };
}

#endif // THREADHELPERS_H
