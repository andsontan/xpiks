#ifndef COMBINEDMODELTESTS_H
#define COMBINEDMODELTESTS_H

#include <QtTest/QtTest>
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/settingsmodel.h"

class CombinedModelTests : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void trivialCombineNoItemsTest();
    void trivialcombineOneItemTest();
    void combineSeveralSameItemsTest();
    void combineAllDifferentItemsTest();
    void combineCommonInKeywordsTest();
    void combineCommonInTitleTest();
    void combineCommonInDescriptionTest();
    void editSeveralWithSameKeywordsTest();
    void recombineAfterRemoveDifferentTest();
    void recombineAfterRemoveAllButOneTest();
    void recombineAfterChangesTest();
    void isNotModifiedAfterTitleDescEditTest();
    void isModifiedAfterKeywordsAppendTest();
    void isModifiedAfterKeywordRemovalTest();
    void isModifiedAfterKeywordEditTest();
    void isModifiedAfterKeywordsClearTest();
    void isNotModifiedAfterEmptyKeywordsClearTest();
    void isModifiedStatusNotResetWithOtherTest();
    void initArtworksEmitsRowsInsertTest();
    void initEmptyArtworksDoesNotEmitTest();
    void initOneArtworkEnablesAllFields();
    void initManyArtworksDoesNotEnableAllFields();
    void resetModelClearsEverythingTest();
    void appendNewKeywordEmitsCountChangedTest();
    void appendExistingKeywordDoesNotEmitTest();
    void pasteNewKeywordsEmitsCountChangedTest();
    void pasteExistingKeywordsDoesNotEmitTest();
    void editKeywordDoesNotEmitCountChangedTest();
    void notSavedAfterAllDisabledTest();
    void notSavedAfterNothingModifiedTest();
    void notSavedAfterModifiedDisabledTest();
    void savedAfterModifiedDescriptionTest();
    void savedAfterModifiedTitleTest();
    void savedAfterKeywordsModifiedTest();
    void savedIfMoreThanOneButNotModifiedTest();
    void caseIsPreservedForOneItemTest();
    void caseIsPreservedForSeveralItemsTest();
    void clearKeywordsFiresKeywordsCountTest();

private:
    Mocks::CommandManagerMock m_CommandManagerMock;
    Models::SettingsModel m_SettingsModel;
};

#endif // COMBINEDMODELTESTS_H
