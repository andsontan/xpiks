#ifndef COMBINEDMODELTESTS_H
#define COMBINEDMODELTESTS_H

#include <QtTest/QtTest>
#include "Mocks/commandmanagermock.h"
#include "../xpiks-qt/Models/settingsmodel.h"

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
    void recombineAfterRemoveDifferentTest();
private:
    Mocks::CommandManagerMock m_CommandManagerMock;
    Models::SettingsModel m_SettingsModel;
};

#endif // COMBINEDMODELTESTS_H
