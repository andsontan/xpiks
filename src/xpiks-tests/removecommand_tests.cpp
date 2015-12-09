#include <QStringList>
#include <QPair>
#include <QSignalSpy>
#include "removecommand_tests.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../xpiks-qt/Commands/removeartworkscommand.h"

void RemoveCommand_Tests::removeArtworksFromEmptyRepository() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtworksRepositoryMock artworksRepositoryMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    commandManagerMock.InjectDependency(artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(0, 2));
    Commands::RemoveArtworksCommand *removeArtworkCommand = new Commands::RemoveArtworksCommand(indicesToRemove);

    QSignalSpy rowsRemovedItemsStart(&artItemsMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedItemsEnd(&artItemsMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy rowsRemovedRepositoryStart(&artworksRepositoryMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedRepositoryEnd(&artworksRepositoryMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy dataChangedInRepository(&artworksRepositoryMock, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    QSignalSpy modifiedFilesChanged(&artItemsMock, SIGNAL(modifiedArtworksCountChanged()));

    Commands::CommandResult *result = commandManagerMock.processCommand(removeArtworkCommand);
    Commands::RemoveArtworksCommandResult *removeArtworksResult = static_cast<Commands::RemoveArtworksCommandResult*>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;
    delete result;

    QCOMPARE(artworksRemovedCount, 0);

    QCOMPARE(rowsRemovedItemsStart.count(), 0);
    QCOMPARE(rowsRemovedItemsEnd.count(), 0);

    QCOMPARE(rowsRemovedRepositoryStart.count(), 0);
    QCOMPARE(rowsRemovedRepositoryEnd.count(), 0);

    QCOMPARE(dataChangedInRepository.count(), 0);
    QCOMPARE(modifiedFilesChanged.count(), 0);
}

void RemoveCommand_Tests::removeAllArtworksFromRepository() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtworksRepositoryMock artworksRepositoryMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    commandManagerMock.InjectDependency(artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(0, itemsToAdd - 1));
    Commands::RemoveArtworksCommand *removeArtworkCommand = new Commands::RemoveArtworksCommand(indicesToRemove);

    QSignalSpy rowsRemovedItemsStart(&artItemsMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedItemsEnd(&artItemsMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy rowsRemovedRepositoryStart(&artworksRepositoryMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedRepositoryEnd(&artworksRepositoryMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy dataChangedInRepository(&artworksRepositoryMock, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    QSignalSpy modifiedFilesChanged(&artItemsMock, SIGNAL(modifiedArtworksCountChanged()));

    Commands::CommandResult *result = commandManagerMock.processCommand(removeArtworkCommand);
    Commands::RemoveArtworksCommandResult *removeArtworksResult = static_cast<Commands::RemoveArtworksCommandResult*>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;
    delete result;

    QCOMPARE(artworksRemovedCount, itemsToAdd);

    QCOMPARE(rowsRemovedItemsStart.count(), 1);
    QList<QVariant> rowsRemovedSpyArguments = rowsRemovedItemsStart.takeFirst();
    QCOMPARE(rowsRemovedSpyArguments.at(1).toInt(), 0);
    QCOMPARE(rowsRemovedSpyArguments.at(2).toInt(), itemsToAdd - 1);

    QCOMPARE(rowsRemovedItemsEnd.count(), 1);
    QList<QVariant> rowsRemovedFinishedSpyArguments = rowsRemovedItemsEnd.takeFirst();
    QCOMPARE(rowsRemovedFinishedSpyArguments.at(1).toInt(), 0);
    QCOMPARE(rowsRemovedFinishedSpyArguments.at(2).toInt(), itemsToAdd - 1);

    QCOMPARE(rowsRemovedRepositoryStart.count(), 1);
    QList<QVariant> repositoriesRemovedSpyArguments = rowsRemovedRepositoryStart.takeFirst();
    QCOMPARE(repositoriesRemovedSpyArguments.at(1).toInt(), 0);
    QCOMPARE(repositoriesRemovedSpyArguments.at(2).toInt(), 0);

    QCOMPARE(rowsRemovedRepositoryEnd.count(), 1);
    QList<QVariant> repositoriesRemovedFinishedSpyArguments = rowsRemovedRepositoryEnd.takeFirst();
    QCOMPARE(repositoriesRemovedFinishedSpyArguments.at(1).toInt(), 0);
    QCOMPARE(repositoriesRemovedFinishedSpyArguments.at(2).toInt(), 0);

    QCOMPARE(dataChangedInRepository.count(), 1);
    QList<QVariant> dataChangedSpyArguments = dataChangedInRepository.takeFirst();
    QCOMPARE(dataChangedSpyArguments.at(1).toInt(), 0);
    // no more directories and rowCount() == 0 in event raising
    QCOMPARE(dataChangedSpyArguments.at(2).toInt(), 0);

    QCOMPARE(modifiedFilesChanged.count(), 1);
}
