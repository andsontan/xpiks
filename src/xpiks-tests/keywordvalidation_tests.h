#ifndef KEYWORDVALIDATIONTESTS_H
#define KEYWORDVALIDATIONTESTS_H

#include <QtTest/QTest>

class KeywordValidationTests : public QObject
{
    Q_OBJECT
private slots:
    void oneLetterKeywordValidTest();
    void keywordWithSpaceIsValidTest();
    void atSymbolIsValidTest();
    void digitsAreValidTest();
    void simpleKeywordIsValidTest();
    void keywordsFromDigitsAreValidTest();
};

#endif // KEYWORDVALIDATIONTESTS_H
