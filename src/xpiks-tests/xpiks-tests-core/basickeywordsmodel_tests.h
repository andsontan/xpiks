#ifndef BASICKEYWORDSMODELTESTS_H
#define BASICKEYWORDSMODELTESTS_H

#include <QtTest/QtTest>
#include "../../xpiks-qt/Common/hold.h"

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
    void editToUpperCaseTest();
    void hasKeywordTest();
    void simpleReplaceTest();
    void descriptionReplaceTest();
    void titleReplaceTest();
    void keywordsReplaceTest();
    void noReplaceCaseSensitiveTest();
    void replaceCaseSensitiveTest();
    void replaceWholeWordsTest();
    void replaceKeywordsWithRemoveTest();
    void removeKeywordsFromSetTest();
    void noneKeywordsRemovedFromSetTest();
    void removeKeywordsCaseSensitiveTest();

private:
    Common::Hold m_FakeHold;
};

#endif // BASICKEYWORDSMODELTESTS_H
