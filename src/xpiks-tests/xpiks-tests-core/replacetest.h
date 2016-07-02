#ifndef REPLACETEST_H
#define REPLACETEST_H

#include <QObject>
#include <QtTest/QtTest>

class ReplaceTest: public QObject
{
    Q_OBJECT
private slots:
    void replaceTrivialTest();
    void noReplaceTrivialTest();
    void caseSensitiveTest();
    void replaceTitleTest();
};

#endif // REPLACETEST_H
