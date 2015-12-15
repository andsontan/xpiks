#include "stringhelpers_tests.h"
#include "../xpiks-qt/Helpers/stringhelper.h"
#include <QStringList>

void StringHelpersTests::splitEmptyStringTest() {
    QString text = "";
    QStringList result;
    Helpers::splitText(text, result);

    QVERIFY(result.isEmpty());
}

void StringHelpersTests::splitStringWithSpacesTest() {
    QString text = "    . , : -";
    QStringList result;
    Helpers::splitText(text, result);

    QVERIFY(result.isEmpty());
}

void StringHelpersTests::splitOneWordTest() {
    QString text = "oneword";
    QStringList result;
    Helpers::splitText(text, result);

    QCOMPARE(result.length(), 1);
    QCOMPARE(result[0], text);
}

void StringHelpersTests::splitOneWordWithSpacesTest() {
    QString text = "  oneword    ";
    QStringList result;
    Helpers::splitText(text, result);

    QCOMPARE(result.length(), 1);
    QCOMPARE(result[0], text.trimmed());
}

void StringHelpersTests::splitSeveralWordsWithSpacesTest() {
    QString text = "  OneworD  and Another anD   one more   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "and" << "Another" << "anD" << "one" << "more";

    QCOMPARE(result, expected);
}

void StringHelpersTests::splitOneLetterWordTest() {
    QString text = "  OneworD b    and Another c anD a  one more   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "b" << "and" << "Another" << "c" << "anD" << "a" << "one" << "more";

    QCOMPARE(result, expected);
}

void StringHelpersTests::splitSeveralWordsWithPunctuationTest() {
    QString text = "  OneworD: & and,Anot$her.anD @ - one more...   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "and" << "Anot$her" << "anD" << "one" << "more";

    QCOMPARE(result, expected);
}
