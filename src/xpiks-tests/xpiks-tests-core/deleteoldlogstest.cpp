#include "deleteoldlogstest.h"

void DeleteOldLogsTest::deleteNoLogsTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs;
    Helpers::getFilesToDelete(logs, 0, logsToDelete);

    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::dontDeleteTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs;

    logs = createOldLogs(49, 50);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createBigLogs(9);
    Helpers::getFilesToDelete(logs, 9*(1*1024*1024), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createTooManyLogs(99);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::deleteOldTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createOldLogs(100, 50);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - 50);
}

void DeleteOldLogsTest::deleteLargeTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createBigLogs(12);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - 2);
}

void DeleteOldLogsTest::deleteManyTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createTooManyLogs(113);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - 13);
}

void DeleteOldLogsTest::deleteCombinedTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete, logs;

    logs << createTooManyLogs(113) << createBigLogs(12) << createOldLogs(100, 50);
    Helpers::getFilesToDelete(logs, 12345, logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - (13 + 12 + 50));
}

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createTooManyLogs(int logsCount) {
    QVector<Helpers::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        logs.append({
                        QString("xpiks-qt-01022015-%1.log").arg(N - 1 - logsCount), // m_FilePath
                        12345, // m_SizeBytes
                        0 // m_AgeDays
                    });
    }

    return logs;
}

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createOldLogs(int logsCount, int startDay) {
    QVector<Helpers::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        int index = N - 1 - logsCount;
        logs.append({
                        QString("xpiks-qt-01022015-%1.log").arg(index), // m_FilePath
                        12345, // m_SizeBytes
                        startDay + index // m_AgeDays
                    });
    }

    return logs;
}

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createBigLogs(int logsCount) {
    QVector<Helpers::FileInfoHolder> logs;
    int N = logsCount;

    while (logsCount--) {
        logs.append({
                        QString("xpiks-qt-01022015-%1.log").arg(N - 1 - logsCount), // m_FilePath
                        1 * 1024 * 1024, // m_SizeBytes
                        0 // m_AgeDays
                    });
    }

    return logs;
}
