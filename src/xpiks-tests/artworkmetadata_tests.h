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
    void freshObjectTest();
    void markModifiedShouldEmitSignalTest();
    void modifiedIsNotMarkedModifiedAgainTest();
    void setDescriptionEmitsModified();
    void setTitleEmitsModified();
    void addNewKeywordsEmitsModified();
    void addExistingKeywordsDoesNotEmitModified();
    void addOneNewKeywordEmitsModified();
    void addOneExistingKeywordDoesNotEmitModified();
    void removeKeywordFromEmptyTest();
    void removeLastKeywordFromEmptyTest();
    void removeActualKeywordTest();
    void removeLastActualKeywordTest();
};

#endif // ARTWORKMETADATA_TESTS_H
