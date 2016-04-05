#ifndef JSONMERGE_TESTS_H
#define JSONMERGE_TESTS_H

#include <QObject>
#include <QtTest/QTest>

class JsonMergeTests : public QObject {
    Q_OBJECT

  private slots:
    void mergeTwoFiles();
};

#endif // JSONMERGE_TESTS_H
