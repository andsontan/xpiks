#ifndef UNDOREDOTESTS_H
#define UNDOREDOTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class UndoRedoTests : public QObject
{
    Q_OBJECT
private slots:
    void undoAddCommandTest();
    void undoUndoAddCommandTest();
    void undoRemoveItemsTest();
    void undoUndoRemoveItemsTest();
    void undoModifyCommandTest();
    void undoUndoModifyCommandTest();
};

#endif // UNDOREDOTESTS_H
