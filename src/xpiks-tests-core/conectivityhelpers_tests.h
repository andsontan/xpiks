#ifndef CONECTIVITYHELPERSTESTS_H
#define CONECTIVITYHELPERSTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class ConectivityHelpersTests : public QObject
{
    Q_OBJECT
private slots:
    void generateMixedFilepathesTest();
    void generateWithoutVectorsTest();
    void generateWithVectorsTest();
};

#endif // CONECTIVITYHELPERSTESTS_H
