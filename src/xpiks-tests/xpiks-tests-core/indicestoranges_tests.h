#ifndef INDICESTORANGES_TESTS_H
#define INDICESTORANGES_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class IndicesToRangesTests : public QObject
{
    Q_OBJECT
private slots:
    void noItemsToEmptyIndicesTest();
    void allRangeIndicesTest();
    void allRangeNotFromZeroTest();
    void allRangeOnlyTwoItemsTest();
    void allRangeOnlyOneItemNotZeroTest();
    void allRangeOnlyOneItemZeroTest();
    void separateRangesSimpleTest();
    void separateRangesWithOneItemInTheBeginningTest();
    void separateRangesWithOneItemInTheMiddleTest();
    void separateRangesWithOneItemInTheEndTest();
    void combinedTest();
};

#endif // INDICESTORANGES_TESTS_H
