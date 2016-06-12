#ifndef DELETEOLDLOGSTEST_H
#define DELETEOLDLOGSTEST_H

#include <QObject>
#include <QtTest/QtTest>
#include "../xpiks-qt/Helpers/deletelogshelper.h"

class DeleteOldLogsTest:
    public QObject
{
Q_OBJECT

private slots:
    void TestEmptyToDelete();
    void TestAllToDelete();
    void TestNoToDelete();
    void TestOldToDelete();
    void TestLargeToDelete();
    void TestManyToDelete();
    void TestCombinedToDelete();

private:
    void setupVector(int N_new, int N_old, int N_large);

private:
    QVector<Helpers::FileInfoHolder> m_Logs;
    QVector<Helpers::FileInfoHolder> m_LogsToDelete;
    int m_TotalSizeB;
};

#endif // DELETEOLDLOGSTEST_H
