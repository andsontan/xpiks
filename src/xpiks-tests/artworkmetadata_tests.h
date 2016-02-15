#ifndef ARTWORKMETADATA_TESTS_H
#define ARTWORKMETADATA_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class ArtworkMetadataTests: public QObject
{
    Q_OBJECT
private slots:
    void initializeOverwriteTest();
    void initializeNoOverwriteButEmptyTest();
    void initializeNoOverwriteNotEmptyTest();
    void initializeNoOverwriteTest();
    void initializeOverwriteWithEmptyTest();
    void freshObjectTest();
    void markModifiedShouldEmitSignalTest();
    void modifiedIsNotMarkedModifiedAgainTest();
    void setDescriptionEmitsModifiedTest();
    void setTitleEmitsModifiedTest();
    void addNewKeywordsEmitsModifiedTest();
    void addExistingKeywordsDoesNotEmitModifiedTest();
    void addOneNewKeywordEmitsModifiedTest();
    void addOneExistingKeywordDoesNotEmitModifiedTest();
    void removeKeywordFromEmptyTest();
    void removeLastKeywordFromEmptyTest();
    void removeActualKeywordTest();
    void removeLastActualKeywordTest();
    void editKeywordToAnotherEmitsModifiedTest();
    void editKeywordToExistingDoesNotEmitModifiedTest();
    void misEditOfKeywordDoesNothingTest();
    void isInDirectoryTest();
    void isNotInParentsDirectoryTest();
    void isNotInOtherDirectoryTest();
    void isNotInEmptyDirectoryTest();
};

#endif // ARTWORKMETADATA_TESTS_H
