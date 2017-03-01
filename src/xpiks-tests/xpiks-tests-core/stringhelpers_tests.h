#ifndef STRINGHELPERSTESTS_H
#define STRINGHELPERSTESTS_H

#include <QtTest/QTest>

class StringHelpersTests : public QObject
{
    Q_OBJECT
private slots:
    void splitEmptyStringTest();
    void splitStringWithSpacesTest();
    void splitOneWordTest();
    void splitOneWordWithSpacesTest();
    void splitSeveralWordsWithSpacesTest();
    void splitOneLetterWordTest();
    void splitSeveralWordsWithPunctuationTest();
    void replaceWholeSmokeTest();
    void replaceWholeTrivialTest();
    void replaceWholeTrivialCaseSensitiveTest();
    void replaceWholePrefixTest();
    void replaceWholeSuffixTest();
    void replaceWholeWithPunctuationTest();
    void replaceWholeWithSpaceTest();
    void replaceWholeWithCommaTest();
    void replaceWholeNoCaseHitTest();
    void replaceWholeNoHitTest();

#ifdef KEYWORDS_TAGS
    void mergeTaggedListsBasicTest();
    void mergeHalfEmptyTaggedListTest();
    void mergeWithDifferentCaseTest();
    void mergeEmtpyWithEmptyTest();
    void mergeEmptyStringWithNonEmptyTest();
    void mergeEmptyStringsTest();
    void mergeWithMissingTagsTest();
    void mergeWithUnorderedTagsTest();
    void mergeOneItemListsTest();
    void mergeWithDifferentCaseTagsTest();
    void mergeWithNonTaggedTest();
    void mergeAllNonTaggedTest();
    void mergeWithDuplicatedTagTest();

    void intersectTaggedListsBasicTest();
    void intersectWithEmptyTest();
    void intersectEmptyWithTest();
    void intersectWithMissingTagsTest();
    void intersectWithJustTagsTest();
    void intersectWithCommonTest();
    void intersectOnlyCommonTest();
    void intersectMixedTest();
#endif
};

#endif // STRINGHELPERSTESTS_H
