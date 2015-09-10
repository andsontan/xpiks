#ifndef VECTORFILENAMES_TESTS_H
#define VECTORFILENAMES_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class VectorFileNamesTests : public QObject
{
    Q_OBJECT
private slots:
    void simpleFilenamesJpgTest();
    void simpleFilenamesTiffTest();
    void filenamesNotReplacedTest();
};

#endif // VECTORFILENAMES_TESTS_H
