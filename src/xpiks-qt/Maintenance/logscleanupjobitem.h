/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LOGSCLEANUPJOBITEM_H
#define LOGSCLEANUPJOBITEM_H

#include <QString>
#include <QVector>
#include "../Common/defines.h"
#include "imaintenanceitem.h"

#define MAX_LOGS_SIZE_MB 10
#define MAX_LOGS_SIZE_BYTES (MAX_LOGS_SIZE_MB * 1024 * 1024)
#define MAX_LOGS_AGE_DAYS 60
#define MAX_LOGS_NUMBER 100

namespace Maintenance {
    struct FileInfoHolder {
        QString m_Filepath;
        qint64 m_SizeBytes;
        int m_AgeDays;
    };

    bool operator <(const FileInfoHolder &arg1, const FileInfoHolder &arg2);

    class LogsCleanupJobItem: public IMaintenanceItem
    {
    public:
        LogsCleanupJobItem();

    public:
        virtual void processJob() override;

#ifdef CORE_TESTS
    public:
#else
    private:
#endif
        static void getFilesToDelete(const QVector<FileInfoHolder> &logFiles, qint64 overallSizeBytes,
                              QVector<FileInfoHolder> &filesToDelete);

    private:
        void doCleanLogs();
        qint64 findLogFiles(const QString &logsDir, QVector<FileInfoHolder> &logFiles);
        void deleteLogsFilesFromList(const QVector<FileInfoHolder> &files);
        void deleteLogFile(const QString &fileNameFull);
        QDateTime getDateFromName(const QString &name);

    private:
        QString m_LogFileDir;
    };
}

#endif // LOGSCLEANUPJOBITEM_H
