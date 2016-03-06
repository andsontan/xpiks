#ifndef FILTEREDMODELTESTS_H
#define FILTEREDMODELTESTS_H

#include <QObject>
#include <QtTest/QTest>

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
    void removeMetadataDeletesMetadataTest();
    void findSelectedIndexTest();
    void clearKeywordsTest();
    void detachVectorFromSelectedTest();
    void setSelectedForUploadTest();
};

#endif // FILTEREDMODELTESTS_H
