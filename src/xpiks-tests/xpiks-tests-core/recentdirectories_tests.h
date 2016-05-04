#ifndef RECENTDIRECTORIESTESTS_H
#define RECENTDIRECTORIESTESTS_H

#include <QObject>
#include <QtTest/QTest>

class RecentDirectoriesTests : public QObject
{
    Q_OBJECT
private slots:
    void pushMoreThanTest();
    void lastPushedIsMostRecent();
};

#endif // RECENTDIRECTORIESTESTS_H
