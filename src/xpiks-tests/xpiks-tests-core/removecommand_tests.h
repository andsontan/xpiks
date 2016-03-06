#ifndef REMOVECOMMAND_TESTS_H
#define REMOVECOMMAND_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class RemoveCommand_Tests : public QObject
{
    Q_OBJECT

private slots:
    void removeArtworksFromEmptyRepository();
    void removeAllArtworksFromRepository();
};

#endif // REMOVECOMMAND_TESTS_H
