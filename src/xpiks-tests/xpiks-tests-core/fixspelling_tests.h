#ifndef FIXSPELLINGTESTS_H
#define FIXSPELLINGTESTS_H

#include <QObject>
#include <QtTest/QtTest>
#include "../../xpiks-qt/Common/hold.h"

class FixSpellingTests: public QObject
{
    Q_OBJECT
private slots:
    void fixKeywordsSmokeTest();
    void noReplacementsSelectedTest();
    void fixAndRemoveDuplicatesTest();
    void fixAndRemoveDuplicatesCombinedTest();
    void multiReplaceWithCorrectAllTest();
    void replaceWithCorrectDescriptionTest();
    void replaceWithCorrectTitleTest();
    void replaceWithCorrectKeywordsTest();

private:
    Common::Hold m_FakeHold;
};

#endif // FIXSPELLINGTESTS_H
