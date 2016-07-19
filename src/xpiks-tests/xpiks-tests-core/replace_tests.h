#ifndef REPLACETEST_H
#define REPLACETEST_H

#include <QObject>
#include <QtTest/QtTest>

class ReplaceTests: public QObject
{
    Q_OBJECT
private slots:
    void replaceTrivialTest();
    void noReplaceTrivialTest();
    void caseSensitiveTest();
    void replaceTitleTest();
    void replaceKeywordsTest();
};

#endif // REPLACETEST_H
