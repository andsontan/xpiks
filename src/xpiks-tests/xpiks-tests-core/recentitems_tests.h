#ifndef RECENTITEMSTESTS_H
#define RECENTITEMSTESTS_H

#include <QObject>
#include <QtTest/QTest>

class RecentItemsTests : public QObject
{
    Q_OBJECT
private slots:
    void pushMoreThanXFilesTest();
    void pushMoreThanXDirectoriesTest();
    void lastPushedIsMostRecentFileTest();
    void lastPushedIsMostRecentDirectoryTest();
};

#endif // RECENTITEMSTESTS_H
