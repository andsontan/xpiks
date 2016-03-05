#include "undoredo_tests.h"
#include <QStringList>
#include <QSignalSpy>
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../xpiks-qt/Commands/addartworkscommand.h"
#include "../xpiks-qt/Commands/removeartworkscommand.h"
#include "../xpiks-qt/Commands/combinededitcommand.h"
#include "../xpiks-qt/Models/settingsmodel.h"
#include "../xpiks-qt/UndoRedo/undoredomanager.h"
#include "../xpiks-qt/Common/flags.h"
#include "../xpiks-qt/Models/artiteminfo.h"

#define SETUP_TEST \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsMock;\
    Models::ArtworksRepository artworksRepository;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    Models::ArtItemsModel *artItemsModel = &artItemsMock;\
    commandManagerMock.InjectDependency(artItemsModel);\
    UndoRedo::UndoRedoManager undoRedoManager;\
    commandManagerMock.InjectDependency(&undoRedoManager);

void UndoRedoTests::undoAddCommandTest() {
    SETUP_TEST;

    QStringList filenames;
    filenames << "/path/to/test/image1.jpg";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, QStringList(), false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, filenames.length());
    QCOMPARE(artItemsMock.getArtworksCount(), filenames.length());

    bool undoSucceeded = undoRedoManager.undoLastAction();
    QVERIFY(undoSucceeded);

    QCOMPARE(artItemsMock.getArtworksCount(), 0);
}

void UndoRedoTests::undoUndoAddCommandTest() {
    SETUP_TEST;

    QStringList filenames;
    filenames << "/path/to/test/image1.jpg";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, QStringList(), false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, filenames.length());
    QCOMPARE(artItemsMock.getArtworksCount(), filenames.length());

    bool undoSucceeded = undoRedoManager.undoLastAction();
    QVERIFY(undoSucceeded);

    undoSucceeded = undoRedoManager.undoLastAction();
    QVERIFY(undoSucceeded);

    QCOMPARE(artItemsMock.getArtworksCount(), filenames.length());
    for (int i = 0; i < filenames.length(); ++i) {
        QCOMPARE(artItemsMock.getArtworkFilepath(i), filenames[i]);
    }
}

void UndoRedoTests::undoRemoveItemsTest() {
    SETUP_TEST;
    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(1, 3));
    Commands::RemoveArtworksCommand *removeArtworkCommand = new Commands::RemoveArtworksCommand(indicesToRemove);
    Commands::ICommandResult *result = commandManagerMock.processCommand(removeArtworkCommand);
    Commands::RemoveArtworksCommandResult *removeArtworksResult = static_cast<Commands::RemoveArtworksCommandResult*>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;
    delete result;

    QCOMPARE(artworksRemovedCount, 3);

    bool undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(undoStatus);

    QCOMPARE(artItemsMock.getArtworksCount(), itemsToAdd);
}

void UndoRedoTests::undoUndoRemoveItemsTest() {
    SETUP_TEST;
    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(1, 3));
    Commands::RemoveArtworksCommand *removeArtworkCommand = new Commands::RemoveArtworksCommand(indicesToRemove);
    Commands::ICommandResult *result = commandManagerMock.processCommand(removeArtworkCommand);
    Commands::RemoveArtworksCommandResult *removeArtworksResult = static_cast<Commands::RemoveArtworksCommandResult*>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;
    delete result;

    QCOMPARE(artworksRemovedCount, 3);

    bool undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(undoStatus);

    undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(undoStatus);

    QCOMPARE(artItemsMock.getArtworksCount(), 2);
}

void UndoRedoTests::undoModifyCommandTest() {
    SETUP_TEST;
    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    QString originalTitle = "title";
    QString originalDescription = "some description here";
    QStringList originalKeywords = QString("test1,test2,test3").split(',');
    QVector<Models::ArtItemInfo*> infos;

    for (int i = 0; i < itemsToAdd; ++i) {
        artItemsMock.getArtwork(i)->initialize(originalTitle, originalDescription, originalKeywords);
        infos.append(new Models::ArtItemInfo(artItemsMock.getArtwork(i), i));
    }

    artItemsMock.getArtwork(0)->setModified();

    int flags = Common::EditEverything;
    QString otherDescription = "brand new description";
    QString otherTitle = "other title";
    QStringList otherKeywords = QString("another,keywords,here").split(',');
    Commands::CombinedEditCommand *combinedEditCommand =
            new Commands::CombinedEditCommand(flags, infos, otherDescription, otherTitle, otherKeywords);
    Commands::ICommandResult *result = commandManagerMock.processCommand(combinedEditCommand);
    Commands::CombinedEditCommandResult *combinedEditResult = static_cast<Commands::CombinedEditCommandResult*>(result);
    QVector<int> indices = combinedEditResult->m_IndicesToUpdate;
    delete result;

    QCOMPARE(indices.length(), itemsToAdd);

    bool undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(undoStatus);

    for (int i = 0; i < itemsToAdd; ++i) {
        Models::ArtworkMetadata *metadata = artItemsMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), originalDescription);
        QCOMPARE(metadata->getTitle(), originalTitle);
        QCOMPARE(metadata->getKeywords(), originalKeywords);
        if (i > 0) {
            QVERIFY(!artItemsMock.getArtwork(i)->isModified());
        }
    }

    QVERIFY(artItemsMock.getArtwork(0)->isModified());
}

void UndoRedoTests::undoUndoModifyCommandTest() {
    SETUP_TEST;
    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    QString originalTitle = "title";
    QString originalDescription = "some description here";
    QStringList originalKeywords = QString("test1,test2,test3").split(',');
    QVector<Models::ArtItemInfo*> infos;

    for (int i = 0; i < itemsToAdd; ++i) {
        artItemsMock.getArtwork(i)->initialize(originalTitle, originalDescription, originalKeywords);
        infos.append(new Models::ArtItemInfo(artItemsMock.getArtwork(i), i));
    }

    artItemsMock.getArtwork(0)->setModified();

    int flags = Common::EditEverything;
    QString otherDescription = "brand new description";
    QString otherTitle = "other title";
    QStringList otherKeywords = QString("another,keywords,here").split(',');
    Commands::CombinedEditCommand *combinedEditCommand =
            new Commands::CombinedEditCommand(flags, infos, otherDescription, otherTitle, otherKeywords);
    Commands::ICommandResult *result = commandManagerMock.processCommand(combinedEditCommand);
    Commands::CombinedEditCommandResult *combinedEditResult = static_cast<Commands::CombinedEditCommandResult*>(result);
    QVector<int> indices = combinedEditResult->m_IndicesToUpdate;
    delete result;

    QCOMPARE(indices.length(), itemsToAdd);

    bool undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(undoStatus);

    undoStatus = undoRedoManager.undoLastAction();
    QVERIFY(!undoStatus);
}
