#ifndef ADDCOMMAND_TESTS_H
#define ADDCOMMAND_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class AddCommand_Tests : public QObject
{
    Q_OBJECT
private slots:
    void addNoArtworksToEmptyRepositoryTest();
    void addOneArtworkToEmptyRepositoryTest();
    void addAndAttachVectorsTest();
    void addAndAttachVectorsLaterTest();
    void addAndDontAttachVectorsOtherDirTest();
    void addAndDontAttachVectorsEmptyDirTest();
    void addAndDontAttachVectorsStartsWithTest();
    void addAndAttachFromSingleDirectoryTest();
    void addSingleDirectoryAndAttachLaterTest();
};

#endif // ADDCOMMAND_TESTS_H
