#ifndef REPLACEPREVIEW_H
#define REPLACEPREVIEW_H

#include <QObject>
#include <QtTest/QtTest>

class ReplacePreviewTest:
        public QObject
{
    Q_OBJECT

private slots:
    void noTructation();
    void simpleTructation();
    void doubleTructation();
    void advancedTructation();
};

#endif // REPLACEPREVIEW_H
