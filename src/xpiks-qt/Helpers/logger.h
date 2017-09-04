/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QString>
#include <QWaitCondition>
#include <QMutex>

namespace Helpers {
    class Logger
    {
    public:
        static Logger& getInstance()
        {
            static Logger instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    public:
        void setLogFilePath(const QString &filepath) {
            m_LogFilepath = filepath;
        }

        QString getLogFilePath() const { return m_LogFilepath; }

        void log(const QString &message);
        void flush();
        void stop();

#ifdef INTEGRATION_TESTS
    public:
        void emergencyFlush();
#endif

    private:
        void doLog(const QString &message);
        void flushStream(QStringList *logItems);

    private:
        Logger() {
            m_QueueLogTo = &m_LogsStorage[0];
            m_QueueFlushFrom = &m_LogsStorage[1];
            m_Stopped = false;
        }

        Logger(Logger const&);
        void operator=(Logger const&);

    private:
        QString m_LogFilepath;
        QStringList m_LogsStorage[2];
        QStringList *m_QueueFlushFrom;
        QStringList *m_QueueLogTo;
        QMutex m_LogMutex;
        QMutex m_FlushMutex;
        QWaitCondition m_AnyLogsToFlush;
        volatile bool m_Stopped;
    };
}

#endif // LOGGER_H
