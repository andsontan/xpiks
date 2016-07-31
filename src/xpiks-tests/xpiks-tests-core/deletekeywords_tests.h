#ifndef DELETEKEYWORDSTESTS_H
#define DELETEKEYWORDSTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class DeleteKeywordsTests : public QObject
{
    Q_OBJECT
private slots:
    void smokeTest();
    void keywordsCombinedTest();
    void doesNotDeleteOtherCaseTest();
    void doesNotDeleteNoKeywordsTest();
};

#endif // DELETEKEYWORDSTESTS_H
