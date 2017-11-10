/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DELAYEDACTIONENTITY_H
#define DELAYEDACTIONENTITY_H

#include "defines.h"
#include <QtGlobal>
#include <QTimerEvent>

namespace Common {
    class DelayedActionEntity {
    public:
        DelayedActionEntity(int timerInterval, int maxRestartsCount):
            m_RestartsCount(0),
            m_LastTimerId(-1),
            m_MaxRestartsCount(maxRestartsCount),
            m_TimerInterval(timerInterval)
        { }

    protected:
        void justChanged() {
            if (m_RestartsCount < m_MaxRestartsCount) {
                if (m_LastTimerId != -1) {
                    doKillTimer(m_LastTimerId);
                    LOG_INTEGR_TESTS_OR_DEBUG << "killed timer" << m_LastTimerId;
                }

                m_LastTimerId = doStartTimer(m_TimerInterval, Qt::VeryCoarseTimer);
                LOG_INTEGR_TESTS_OR_DEBUG << "started timer" << m_LastTimerId;
                m_RestartsCount++;
            } else {
                Q_ASSERT(m_LastTimerId != -1);
                LOG_INFO << "Maximum backup delays occured, forcing backup";
            }
        }

        void onQtTimer(QTimerEvent *event) {
            if (event == nullptr) { return; }

            LOG_DEBUG << "timer" << event->timerId();

            if (event->timerId() == m_LastTimerId) {
                m_RestartsCount = 0;
                m_LastTimerId = -1;

                doOnTimer();

                // one time event
                doKillTimer(event->timerId());
            } else {
                callBaseTimer(event);
            }
        }

    protected:
        virtual void doKillTimer(int timerId) = 0;
        virtual int doStartTimer(int interval, Qt::TimerType timerType) = 0;
        virtual void doOnTimer() = 0;
        virtual void callBaseTimer(QTimerEvent *event) = 0;

    private:
        int m_RestartsCount;
        int m_LastTimerId;
        const int m_MaxRestartsCount;
        const int m_TimerInterval;
    };
}

#endif // DELAYEDACTIONENTITY_H
