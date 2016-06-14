#include "deleteoldlogstest.h"

qint64 getFilesSize(const QVector<Helpers::FileInfoHolder> &files) {
    qint64 sum = 0;
    int size = files.size();
    for (int i = 0; i < size; ++i) {
        sum += files[i].m_SizeBytes;
    }
    return sum;
}

void DeleteOldLogsTest::deleteNoLogsTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs;
    Helpers::getFilesToDelete(logs, 0, logsToDelete);

    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::dontDeleteTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs;

    logs = createOldLogs(60, 0);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createBigLogs(9);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);

    logs = createTooManyLogs(99);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);
    QCOMPARE(logsToDelete.size(), 0);
}

void DeleteOldLogsTest::deleteOldTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createOldLogs(121, 0);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), logs.size() - 60);
}

void DeleteOldLogsTest::deleteLargeTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createBigLogs(12);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), 3);
}

void DeleteOldLogsTest::deleteManyTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete;

    QVector<Helpers::FileInfoHolder> logs = createTooManyLogs(113);
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    QCOMPARE(logsToDelete.size(), 13);
}

void DeleteOldLogsTest::deleteCombinedTest() {
    QVector<Helpers::FileInfoHolder> logsToDelete, logs;

    logs << createTooManyLogs(100) << createBigLogs(12) << createOldLogs(70, 60);
    std::sort(logs.begin(), logs.end());
    Helpers::getFilesToDelete(logs, getFilesSize(logs), logsToDelete);

    // logs removed until few big logs are removed
    QCOMPARE(logsToDelete.size(), (100 + 70 + 3));
}

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createTooManyLogs(int logsCount) {
    QVector<Helpers::FileInfoHolder> logs;
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

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createOldLogs(int logsCount, int startDay) {
    QVector<Helpers::FileInfoHolder> logs;
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

QVector<Helpers::FileInfoHolder> DeleteOldLogsTest::createBigLogs(int logsCount) {
    QVector<Helpers::FileInfoHolder> logs;
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
