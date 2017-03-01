#include "stringhelpers_tests.h"
#include "../../xpiks-qt/Helpers/stringhelper.h"
#include <QStringList>
#include "../../xpiks-qt/Common/defines.h"

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

void StringHelpersTests::replaceWholeSmokeTest() {
    QString text = "Word";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob");
    QCOMPARE(replaced, QLatin1String("Bob"));
}

void StringHelpersTests::replaceWholeTrivialTest() {
    QString text = "a whole  Word  heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "Word", "Bob");
    QCOMPARE(replaced, QLatin1String("a whole  Bob  heWordre"));
}

void StringHelpersTests::replaceWholeTrivialCaseSensitiveTest() {
    QString text = "a whwordole  Word word heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, QLatin1String("a whwordole  Word Bob heWordre"));
}

void StringHelpersTests::replaceWholePrefixTest() {
    QString text = "Word whole  Words heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "Word", "Bob");
    QCOMPARE(replaced, QLatin1String("Bob whole  Words heWordre"));
}

void StringHelpersTests::replaceWholeSuffixTest() {
    QString text = "Word whole Words word";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, QLatin1String("Word whole Words Bob"));
}

void StringHelpersTests::replaceWholeWithPunctuationTest() {
    QString text = "Word whole, Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob, Words word"));
}

void StringHelpersTests::replaceWholeWithSpaceTest() {
    QString text = "Word whole  Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole ", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob Words word"));
}

void StringHelpersTests::replaceWholeWithCommaTest() {
    QString text = "Word whole;, Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole;", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob, Words word"));
}

void StringHelpersTests::replaceWholeNoCaseHitTest() {
    QString text = "Word inWord and the Wordend";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, text);
}

void StringHelpersTests::replaceWholeNoHitTest() {
    QString text = "Word inWord and the Wordend";
    QString replaced = Helpers::replaceWholeWords(text, "whole", "Bob");
    QCOMPARE(replaced, text);
}

#ifdef KEYWORDS_TAGS

void StringHelpersTests::mergeTaggedListsBasicTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 in addition #t2 these as well #t3 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 some keywords here in addition #t2 few more there these as well #t3 yum this that").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeHalfEmptyTaggedListTest() {
    QStringList list1 = QString("#t1 some keywords here #t2").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 #t2 these as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 some keywords here #t2 these as well").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithDifferentCaseTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 As Well").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 Some Here #t2 these as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 some keywords here Some Here #t2 As Well these as well").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeEmtpyWithEmptyTest() {
    QStringList list1 = QString("#t1 #t2").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 #t2").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 #t2").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeEmptyStringWithNonEmptyTest() {
    QStringList list1 = QString("").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 #t2 these as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 #t2 these as well").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeEmptyStringsTest() {
    QStringList list1 = QString("").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithMissingTagsTest() {
    QStringList list1 = QString("#t1 some keywords here #t3 few more there #t4").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t0 in addition #t2 these as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t0 in addition #t1 some keywords here #t2 these as well #t3 few more there #t4").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithUnorderedTagsTest() {
    QStringList list1 = QString("#t2 some keywords here #t1 few more there #t3 test").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 in addition #t3 keyword #t2 these as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 few more there in addition #t2 some keywords here these as well #t3 test keyword").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeOneItemListsTest() {
    QStringList list1 = QString("#t1 test").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 keyword").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 test keyword").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithDifferentCaseTagsTest() {
    QStringList list1 = QString("#T1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 in addition #T2 these as well #T3 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 some keywords here in addition #t2 few more there these as well #t3 yum this that").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithNonTaggedTest() {
    QStringList list1 = QString("non tagged words #t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("absolutely #t1 in addition #t2 these as well #t3 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("non tagged words absolutely #t1 some keywords here in addition #t2 few more there these as well #t3 yum this that").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeAllNonTaggedTest() {
    QStringList list1 = QString("non tagged words").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("absolutely tagged non").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("non tagged words absolutely tagged non").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::mergeWithDuplicatedTagTest() {
    QStringList list1 = QString("#t1 some keywords here #t1 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 in addition #t2 these as well #t1 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 some keywords here few more there in addition that #t2 these as well #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::mergeTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectTaggedListsBasicTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 and here #t2 there as well #t3 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 here #t2 there #t3").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectWithEmptyTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2;

    const QStringList expected;
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectEmptyWithTest() {
    QStringList list1;
    QStringList list2 = QString("#t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);

    const QStringList expected;
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectWithMissingTagsTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 completely new #t3 few more there").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 and here #t3 there as well").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 here #t3 there").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);
    LOG_FOR_TESTS << actual;

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectWithJustTagsTest() {
    QStringList list1 = QString("#t1 some keywords here #t2 completely new #t3 few more there").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("#t1 #t3").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("#t1 #t3").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectWithCommonTest() {
    QStringList list1 = QString("completely irrelevant #t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("irrelevant keywords #t1 and here #t2 there as well #t3 that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("irrelevant #t1 here #t2 there #t3").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectOnlyCommonTest() {
    QStringList list1 = QString("completely irrelevant some keywords here few more there yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("irrelevant keywords and here there as well that").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("irrelevant keywords here there").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);
    LOG_FOR_TESTS << actual;

    QCOMPARE(actual, expected);
}

void StringHelpersTests::intersectMixedTest() {
    QStringList list1 = QString("completely irrelevant #t1 some keywords here #t2 few more there #t3 yum this").split(" ", QString::SkipEmptyParts);
    QStringList list2 = QString("irrelevant keywords").split(" ", QString::SkipEmptyParts);

    const QStringList expected = QString("irrelevant").split(" ", QString::SkipEmptyParts);
    QStringList actual = Helpers::intersectTaggedLists(list1, list2);

    QCOMPARE(actual, expected);
}

#endif
