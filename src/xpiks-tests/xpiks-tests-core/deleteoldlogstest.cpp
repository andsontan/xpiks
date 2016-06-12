#include "deleteoldlogstest.h"
#include <QtAlgorithms>

int size = criticalLogsNumber / 10 + 1;
bool operator ==(const Helpers::FileInfoHolder &arg1, const Helpers::FileInfoHolder &arg2);

void DeleteOldLogsTest::TestEmptyToDelete() {
    DeleteOldLogsTest::setupVector(0, 0, 0);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestAllToDelete() {
    DeleteOldLogsTest::setupVector(0, criticalLogsNumber / 2 + 1, criticalLogsNumber / 2 + 1);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestNoToDelete() {
    DeleteOldLogsTest::setupVector(size, 0, 0);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestOldToDelete() {
    DeleteOldLogsTest::setupVector(size, size, 0);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestLargeToDelete() {
    DeleteOldLogsTest::setupVector(size, 0, size);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestManyToDelete() {
    DeleteOldLogsTest::setupVector(criticalLogsNumber + size, 0, 0);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::TestCombinedToDelete() {
    DeleteOldLogsTest::setupVector(size, size, size);

    QVector<Helpers::FileInfoHolder> deletedFiles = Helpers::getFilesToDelete(m_Logs, m_TotalSizeB);
    int size = deletedFiles.size();
    QCOMPARE(size, m_LogsToDelete.size());
    qSort(deletedFiles.begin(), deletedFiles.end());
    for (int i = 0; i < size; i++) {
        QCOMPARE(deletedFiles[i], m_LogsToDelete[i]);
    }
}

void DeleteOldLogsTest::setupVector(int N_new, int N_old, int N_large) {
    Helpers::FileInfoHolder file;

    m_TotalSizeB = 0;
    m_LogsToDelete.resize(0);
    m_Logs.resize(0);

    for (int i = 0; i < N_old; i++) {
        file.days = critialLogsAgeDays + 1;
        file.size = i;
        file.name = QStringLiteral("fileD%1").arg(i);
        m_LogsToDelete.append(file);
        m_Logs.append(file);
        m_TotalSizeB += file.size;
    }

    for (int i = 0; i < N_large; i++) {
        file.days = i;
        file.size = criticalLogsSizeB + 1;
        file.name = QStringLiteral("fileD%1").arg(i + N_old);
        m_LogsToDelete.append(file);
        m_Logs.append(file);
        m_TotalSizeB += file.size;
    }

    for (int i = 0; i < N_new; i++) {
        file.days = 0;
        file.size = 0;
        file.name = "fileH";
        m_Logs.append(file);
        if ((i + N_large + N_old) >= criticalLogsNumber) {
            m_LogsToDelete.append(file);
        }

        m_TotalSizeB += file.size;
    }

    qSort(m_Logs.begin(), m_Logs.end());
    qSort(m_LogsToDelete.begin(), m_LogsToDelete.end());
}

bool operator ==(const Helpers::FileInfoHolder &arg1, const Helpers::FileInfoHolder &arg2) {
    return ((arg1.days == arg2.days) && (arg1.name == arg2.name) && (arg1.size == arg2.size));
}
