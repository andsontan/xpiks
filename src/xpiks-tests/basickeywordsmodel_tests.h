#ifndef BASICKEYWORDSMODELTESTS_H
#define BASICKEYWORDSMODELTESTS_H

#include <QtTest/QtTest>

class BasicKeywordsModelTests: public QObject
{
    Q_OBJECT
private slots:
    void constructEmptyTest();
    void simpleAddKeywordTest();
    void addExistingKeywordTest();
    void addSeveralKeywordsTest();
    void removeExistingKeywordTest();
    void removeNonExistingKeywordTest();
    void appendSameKeywordsTest();
    void appendSameChangedKeywordTest();
    void appendNoKeywordsTest();
    void clearKeywordsTest();
    void clearModelTest();
    void containsKeywordTest();
    void containsKeywordStrictTest();
    void containsKeywordFuzzyTest();
    void doesNotContainKeywordTest();
    void setSameTitleTest();
    void setSameDescriptionTest();
    void editKeywordToAnotherTest();
    void editKeywordToSameTest();
    void addRemoveAddUpperCaseWordTest();
};

#endif // BASICKEYWORDSMODELTESTS_H
