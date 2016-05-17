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
    void undoUndoAddWithVectorsTest();
    void undoRemoveItemsTest();
    void undoUndoRemoveItemsTest();
    void undoModifyCommandTest();
    void undoUndoModifyCommandTest();
    void undoPasteCommandTest();
    void undoClearAllTest();
    void undoClearKeywordsTest();
};

#endif // UNDOREDOTESTS_H
