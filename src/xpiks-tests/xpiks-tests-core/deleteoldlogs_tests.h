#ifndef DELETEOLDLOGSTEST_H
#define DELETEOLDLOGSTEST_H

#include <QObject>
#include <QtTest/QtTest>
#include "../../xpiks-qt/Maintenance/logscleanupjobitem.h"

class DeleteOldLogsTest: public QObject
{
    Q_OBJECT
private slots:
    void deleteNoLogsTest();
    void dontDeleteTest();
    void deleteOldTest();
    void deleteLargeTest();
    void deleteManyTest();
    void deleteCombinedTest();

private:
    QVector<Maintenance::FileInfoHolder> createTooManyLogs(int logsCount);
    QVector<Maintenance::FileInfoHolder> createOldLogs(int logsCount, int startDay);
    QVector<Maintenance::FileInfoHolder> createBigLogs(int logsCount);
    QVector<Maintenance::FileInfoHolder> createLogs(int freshLogsCount, int oldLogsCount, int largeLogsCount);

private:
    QVector<Maintenance::FileInfoHolder> m_Logs;
    QVector<Maintenance::FileInfoHolder> m_LogsToDelete;
    int m_TotalSizeB;
};

#endif // DELETEOLDLOGSTEST_H
