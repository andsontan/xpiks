#ifndef FILTEREDMODELTESTS_H
#define FILTEREDMODELTESTS_H


#include <QtTest/QTest>
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../xpiks-qt/Models/artworksrepository.h"

class FilteredModelTests : public QObject
{
    Q_OBJECT
private slots:
    void invertSelectionForEmptyTest();
    void invertSelectionForAllSelectedTest();
    void invertSelectionForSingleTest();
    void invertSelectionForHalfSelectedTest();
    void invertSelectionForEvenCountTest();
    void invertSelectionForOddCountTest();
    void removeMetadataMarksAsModifiedTest();
};

#endif // FILTEREDMODELTESTS_H
