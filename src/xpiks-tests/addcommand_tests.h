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
};

#endif // ADDCOMMAND_TESTS_H
