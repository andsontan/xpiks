#ifndef REPLACEPREVIEW_H
#define REPLACEPREVIEW_H

#include <QObject>
#include <QtTest/QtTest>

class ReplacePreviewTests: public QObject
{
    Q_OBJECT
private slots:
    void noTructationTest();
    void simpleTructationTest();
    void doubleTructationTest();
    void advancedTructationTest();
};

#endif // REPLACEPREVIEW_H
