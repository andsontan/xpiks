#ifndef DELETEOLDLOGSTEST_H
#define DELETEOLDLOGSTEST_H

#include <QObject>
#include <QtTest/QtTest>
#include "../../xpiks-qt/Helpers/deletelogshelper.h"

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
    QVector<Helpers::FileInfoHolder> createTooManyLogs(int logsCount);
    QVector<Helpers::FileInfoHolder> createOldLogs(int logsCount, int startDay);
    QVector<Helpers::FileInfoHolder> createBigLogs(int logsCount);
    QVector<Helpers::FileInfoHolder> createLogs(int freshLogsCount, int oldLogsCount, int largeLogsCount);

private:
    QVector<Helpers::FileInfoHolder> m_Logs;
    QVector<Helpers::FileInfoHolder> m_LogsToDelete;
    int m_TotalSizeB;
};

#endif // DELETEOLDLOGSTEST_H
