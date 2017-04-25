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
