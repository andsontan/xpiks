#include "replacepreview_tests.h"
#include "../../xpiks-qt/Helpers/stringhelper.h"
#include"../../xpiks-qt/Common/defines.h"
#include "stringhelpersfortests.h"

void ReplacePreviewTests::smokeTest() {
    QString text = "  keyword  ";

    QString united = Helpers::getUnitedHitsString(text, {2}, 10);
    QCOMPARE(united, text);
}

void ReplacePreviewTests::simpleEntryTest() {
    QString text = "0123456789 keyword 0123456789 0123456789 keyword";

    QString united = Helpers::getUnitedHitsString(text, {11, text.size() - QLatin1String("keyword").size()}, 5);
    QCOMPARE(united, QLatin1String("0123456789 keyword 0123456789 ... 0123456789 keyword"));
}

void ReplacePreviewTests::overlappingEntryTest() {
    QString text = "keyword 0123456789 keyword";

    QString united = Helpers::getUnitedHitsString(text, {0, text.size() - QLatin1String("keyword").size()}, 7);
    QCOMPARE(united, text);
}

void ReplacePreviewTests::advancedEntryTest() {
    QString text = "keyword 0123456789 keyword 0123456789 0123456789 keyword";

    QString united = Helpers::getUnitedHitsString(text, {0, 19, text.size() - QLatin1String("keyword").size()}, 7);
    QCOMPARE(united, QLatin1String("keyword 0123456789 keyword 0123456789 ... 0123456789 keyword"));
}

void ReplacePreviewTests::hitInTheMiddleTest() {
    QString text = "0123456789 keyword 0123456789 0123456789 keyword";

    QString united = Helpers::getUnitedHitsString(text, {14, text.size() - QLatin1String("word").size()}, 3);
    QCOMPARE(united, QLatin1String("0123456789 keyword 0123456789 ... 0123456789 keyword"));
}
