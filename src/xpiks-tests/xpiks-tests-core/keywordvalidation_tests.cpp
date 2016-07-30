#include "keywordvalidation_tests.h"
#include "../../xpiks-qt/Helpers/keywordshelpers.h"

void KeywordValidationTests::oneLetterKeywordValidTest() {
    for (int i = 0; i < 26; ++i) {
        QVERIFY(Helpers::isValidKeyword(QChar('a' + i)));
    }
}

void KeywordValidationTests::keywordWithSpaceIsValidTest() {
    QVERIFY(Helpers::isValidKeyword("Some keyword with space"));
}

void KeywordValidationTests::atSymbolIsValidTest() {
    QVERIFY(Helpers::isValidKeyword("@"));
}

void KeywordValidationTests::digitsAreValidTest() {
    for (int i = 0; i < 9; ++i) {
        QVERIFY(Helpers::isValidKeyword(QChar('0' + i)));
    }
}

void KeywordValidationTests::simpleKeywordIsValidTest() {
    QVERIFY(Helpers::isValidKeyword("samplekeyword"));
}

void KeywordValidationTests::keywordsFromDigitsAreValidTest() {
    QVERIFY(Helpers::isValidKeyword("2015"));
}

void KeywordValidationTests::sanitizeKeywordWithBackCommaTest() {
    QCOMPARE(Helpers::doSanitizeKeyword(" keyword , , "), QLatin1String("keyword"));
}

void KeywordValidationTests::sanitizeKeywordWithSpacesTest() {
    QCOMPARE(Helpers::doSanitizeKeyword("    keyword  another "), QLatin1String("keyword another"));
}

void KeywordValidationTests::sanitizeKeywordWithFrontCommaTest() {
    QCOMPARE(Helpers::doSanitizeKeyword(" ,keyword "), QLatin1String("keyword"));
}

void KeywordValidationTests::sanitizeKeywordWithAllowedComplexTest() {
    QCOMPARE(Helpers::doSanitizeKeyword("  &  ,keyword  another $ "), QLatin1String("& ,keyword another $"));
}

void KeywordValidationTests::sanitizeOneLetterKeywordTest() {
    QCOMPARE(Helpers::doSanitizeKeyword(" ,$ , , "), QLatin1String("$"));
    QCOMPARE(Helpers::doSanitizeKeyword("@"), QLatin1String("@"));
    QCOMPARE(Helpers::doSanitizeKeyword("#  "), QLatin1String("#"));
    QCOMPARE(Helpers::doSanitizeKeyword(" %"), QLatin1String("%"));
}
