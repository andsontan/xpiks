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
};

#endif // STRINGHELPERSTESTS_H
