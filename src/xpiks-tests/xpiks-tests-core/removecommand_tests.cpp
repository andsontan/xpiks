#include <QStringList>
#include <QPair>
#include <QSignalSpy>
#include "removecommand_tests.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../../xpiks-qt/Commands/removeartworkscommand.h"
#include "../../xpiks-qt/Models/artworksrepository.h"

void RemoveCommandTests::removeArtworksFromEmptyRepository() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(0, 2));
    std::shared_ptr<Commands::RemoveArtworksCommand> removeArtworkCommand(new Commands::RemoveArtworksCommand(indicesToRemove));

    QSignalSpy rowsRemovedItemsStart(&artItemsMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedItemsEnd(&artItemsMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy rowsRemovedRepositoryStart(&artworksRepository, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedRepositoryEnd(&artworksRepository, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy dataChangedInRepository(&artworksRepository, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    QSignalSpy modifiedFilesChanged(&artItemsMock, SIGNAL(modifiedArtworksCountChanged()));

    auto result = commandManagerMock.processCommand(removeArtworkCommand);
    auto removeArtworksResult = std::dynamic_pointer_cast<Commands::RemoveArtworksCommandResult>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;

    QCOMPARE(artworksRemovedCount, 0);

    QCOMPARE(rowsRemovedItemsStart.count(), 0);
    QCOMPARE(rowsRemovedItemsEnd.count(), 0);

    QCOMPARE(rowsRemovedRepositoryStart.count(), 0);
    QCOMPARE(rowsRemovedRepositoryEnd.count(), 0);

    QCOMPARE(dataChangedInRepository.count(), 0);
    QCOMPARE(modifiedFilesChanged.count(), 0);
}

void RemoveCommandTests::removeAllArtworksFromRepository() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    int itemsToAdd = 5;
    commandManagerMock.generateAndAddArtworks(itemsToAdd);

    int dirsCount = artworksRepository.getArtworksSourcesCount();

    QVector<QPair<int, int> > indicesToRemove;
    indicesToRemove.append(qMakePair(0, itemsToAdd - 1));
    std::shared_ptr<Commands::RemoveArtworksCommand> removeArtworkCommand(new Commands::RemoveArtworksCommand(indicesToRemove));

    QSignalSpy rowsRemovedItemsStart(&artItemsMock, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedItemsEnd(&artItemsMock, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy rowsRemovedRepositoryStart(&artworksRepository, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    QSignalSpy rowsRemovedRepositoryEnd(&artworksRepository, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QSignalSpy dataChangedInRepository(&artworksRepository, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    QSignalSpy modifiedFilesChanged(&artItemsMock, SIGNAL(modifiedArtworksCountChanged()));

    auto result = commandManagerMock.processCommand(removeArtworkCommand);
    auto removeArtworksResult = std::dynamic_pointer_cast<Commands::RemoveArtworksCommandResult>(result);
    int artworksRemovedCount = removeArtworksResult->m_RemovedArtworksCount;

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
    QCOMPARE(repositoriesRemovedSpyArguments.at(2).toInt(), dirsCount - 1);

    QCOMPARE(rowsRemovedRepositoryEnd.count(), 1);
    QList<QVariant> repositoriesRemovedFinishedSpyArguments = rowsRemovedRepositoryEnd.takeFirst();
    QCOMPARE(repositoriesRemovedFinishedSpyArguments.at(1).toInt(), 0);
    QCOMPARE(repositoriesRemovedFinishedSpyArguments.at(2).toInt(), dirsCount - 1);

    // we get additional signal, because before deleting the direcory, we unselect it
    QCOMPARE(dataChangedInRepository.count(), 2);
    QList<QVariant> dataChangedSpyArguments = dataChangedInRepository.takeFirst();
    QCOMPARE(dataChangedSpyArguments.at(1).toInt(), 0);
    // no more directories and rowCount() == 0 in event raising
    QCOMPARE(dataChangedSpyArguments.at(2).toInt(), 0);

    QCOMPARE(modifiedFilesChanged.count(), 1);
}
