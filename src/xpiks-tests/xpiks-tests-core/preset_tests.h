#ifndef PRESETTESTS_H
#define PRESETTESTS_H


#include <QObject>
#include <QtTest/QtTest>

class PresetTests: public QObject
{
    Q_OBJECT
private slots:
    void expandFromPresetTrivial();
    void expandFromPresetWithDublicates();
    void appendFromPresetTrivial();
    void appendFromPresetWithDublicates();
};

#endif // PRESETTESTS_H
