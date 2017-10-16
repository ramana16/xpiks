#include "deleteoldlogs_tests.h"

qint64 getFilesSize(const QVector<Maintenance::FileInfoHolder> &files) {
    qint64 sum = 0;
    int size = files.size();
    for (int i = 0; i < size; ++i) {
        sum += files[i].m_SizeBytes;
    }
    return sum;
}

void DeleteOldLogsTest::deleteNoLogsTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete;

    QVector<Maintenance::FileInfoHolder> logs;
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, 0, logsToDelete);

    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::dontDeleteTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete;

    QVector<Maintenance::FileInfoHolder> logs;

    logs = createOldLogs(60, 0);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createBigLogs(9);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createTooManyLogs(99);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::deleteOldTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete;

    QVector<Maintenance::FileInfoHolder> logs = createOldLogs(121, 0);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - 60);
}

void DeleteOldLogsTest::deleteLargeTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete;

    QVector<Maintenance::FileInfoHolder> logs = createBigLogs(12);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), 3);
}

void DeleteOldLogsTest::deleteManyTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete;

    QVector<Maintenance::FileInfoHolder> logs = createTooManyLogs(113);
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), 13);
}

void DeleteOldLogsTest::deleteCombinedTest() {
    QVector<Maintenance::FileInfoHolder> logsToDelete, logs;

    logs << createTooManyLogs(100) << createBigLogs(12) << createOldLogs(70, 60);
    std::sort(logs.begin(), logs.end());
    Maintenance::LogsCleanupJobItem::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    // logs removed until few big logs are removed
    QCOMPARE(logsToDelete.size(), (100 + 70 + 3));
}

QVector<Maintenance::FileInfoHolder> DeleteOldLogsTest::createTooManyLogs(int logsCount) {
    QVector<Maintenance::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        logs.append({
                        QString("xpiks-qt-01022015-%1-many.log").arg(N - 1 - logsCount), // m_FilePath
                        1, // m_SizeBytes
                        1 // m_AgeDays
                    });
    }

    return logs;
}

QVector<Maintenance::FileInfoHolder> DeleteOldLogsTest::createOldLogs(int logsCount, int startDay) {
    QVector<Maintenance::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        int index = N - 1 - logsCount;
        logs.append({
                        QString("xpiks-qt-01022015-%1-old.log").arg(index), // m_FilePath
                        1, // m_SizeBytes
                        startDay + index // m_AgeDays
                    });
    }

    return logs;
}

QVector<Maintenance::FileInfoHolder> DeleteOldLogsTest::createBigLogs(int logsCount) {
    QVector<Maintenance::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        logs.append({
                        QString("xpiks-qt-01022015-%1-big.log").arg(N - 1 - logsCount), // m_FilePath
                        1 * 1024 * 1024, // m_SizeBytes
                        0 // m_AgeDays
                    });
    }

    return logs;
}
