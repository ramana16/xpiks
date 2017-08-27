/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "threadhelpers.h"

namespace Helpers {
    ManualResetEvent::ManualResetEvent():
        m_Flag(false)
    {
    }

    void ManualResetEvent::set() {
        QMutexLocker locker(&m_Mutex);
        Q_UNUSED(locker);
        if (m_Flag == false) {
            m_Flag = true;
            m_WaitCondition.wakeAll();
        }
    }

    void ManualResetEvent::waitOne() {
        QMutexLocker locker(&m_Mutex);
        Q_UNUSED(locker);

        while (!m_Flag) {
            m_WaitCondition.wait(&m_Mutex);
        }
    }

    Barrier::Barrier(int threadsNumber):
        m_Turnstile(0),
        m_Turnstile2(0),
        m_ThreadsNumber(threadsNumber),
        m_Count(0)
    {
    }

    void Barrier::wait() {
        phase1();
        phase2();
    }

    void Barrier::phase1() {
        m_Mutex.lock();
        {
            m_Count++;
            if (m_Count == m_ThreadsNumber) {
                m_Turnstile.release(m_ThreadsNumber);
            }
        }
        m_Mutex.unlock();

        m_Turnstile.acquire();
    }

    void Barrier::phase2() {
        m_Mutex.lock();
        {
            m_Count--;
            if (m_Count == 0) {
                m_Turnstile2.release(m_ThreadsNumber);
            }
        }
        m_Mutex.unlock();

        m_Turnstile2.acquire();
    }
}
