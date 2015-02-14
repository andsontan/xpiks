#include "indicestoranges_tests.h"
#include "../xpiks-qt/Helpers/indiceshelper.h"

#include <QDebug>
#include <QList>
#include <QPair>
#include <cstdarg>

typedef QPair<int, int> Pair;
typedef QList<Pair > Pairs;
typedef QList<int> Indices;

Pairs MAKE_PAIRS(int length, ...) {
    Pairs pairs;

    va_list args;
    va_start(args, length);

    for (int i = 0; i < length; ++i) {
        int first = va_arg(args, int);
        int second = va_arg(args, int);

        pairs << Pair(first, second);
    }

    va_end(args);

    return pairs;
}

void COMPARE_PAIRS(const Pairs &actual, const Pairs &expected) {
    if (actual.length() == expected.length()) {
        int actualLength = actual.length();

        for (int i = 0; i < actualLength; ++i) {
            const Pair &actualPair = actual.at(i);
            const Pair &expectedPair = expected.at(i);

            if ((actualPair.first != expectedPair.first) ||
                    (actualPair.second != expectedPair.second)) {
                QFAIL(QString("(%1, %2) was expected, but (%3, %4) found")
                      .arg(expectedPair.first)
                      .arg(expectedPair.second)
                      .arg(actualPair.first)
                      .arg(actualPair.second)
                      .toStdString().c_str());
                break;
            }
        }
    } else {
        QFAIL(QString("Length was different. %1 received but %2 expected")
              .arg(actual.length())
              .arg(expected.length())
              .toStdString().c_str());
    }
}

void IndicesToRangesTests::noItemsToEmptyIndicesTest()
{
    Indices indices;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs;

    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::allRangeIndicesTest()
{
    Indices indices = Indices() << 0 << 1 << 2 << 3 << 4 << 5;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(1, 0, 5);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::allRangeNotFromZeroTest()
{
    Indices indices = Indices() << 1 << 2 << 3 << 4 << 5;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(1, 1, 5);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::allRangeOnlyTwoItemsTest()
{
    Indices indices = Indices() << 4 << 5;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(1, 4, 5);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::allRangeOnlyOneItemNotZeroTest()
{
    Indices indices = Indices() << 4;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(1, 4, 4);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::allRangeOnlyOneItemZeroTest()
{
    Indices indices = Indices() << 0;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(1, 0, 0);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::separateRangesSimpleTest()
{
    Indices indices = Indices() << 1 << 2 << 4 << 5;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(2, 1, 2, 4, 5);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::separateRangesWithOneItemInTheBeginningTest()
{
    Indices indices = Indices() << 1 << 4 << 5;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(2, 1, 1, 4, 5);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::separateRangesWithOneItemInTheMiddleTest()
{
    Indices indices = Indices() << 0 << 1 << 3 << 5 << 6;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(3, 0, 1, 3, 3, 5, 6);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::separateRangesWithOneItemInTheEndTest()
{
    Indices indices = Indices() << 0 << 1 << 2 << 3 << 6;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(2, 0, 3, 6, 6);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}

void IndicesToRangesTests::combinedTest()
{
    Indices indices = Indices() << 0 << 1 << 2 << 4 << 6 << 7 << 10 << 13;
    Pairs actualPairs;
    Helpers::indicesToRanges(indices, actualPairs);

    Pairs expectedPairs = MAKE_PAIRS(5, 0, 2, 4, 4, 6, 7, 10, 10, 13, 13);
    COMPARE_PAIRS(actualPairs, expectedPairs);
}
