#include "keywordvalidation_tests.h"
#include "../xpiks-qt/Helpers/keywordvalidator.h"

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
