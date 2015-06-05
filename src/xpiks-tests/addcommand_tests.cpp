#include <QStringList>
#include <QSignalSpy>
#include "addcommand_tests.h"
#include "../Mocks/commandmanagermock.h"
#include "../Mocks/artitemsmodelmock.h"
#include "../xpiks-qt/Commands/addartworkscommand.h"

void AddCommand_Tests::addOneArtworkTest()
{
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtworksRepositoryMock artworksRepositoryMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    commandManagerMock.InjectDependency(artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QSignalSpy artItemsBeginInsertSpy(&artItemsMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artItemsEndInsertSpy(&artItemsMock, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoBeginInsertSpy(&artworksRepositoryMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoEndInsertSpy(&artworksRepositoryMock, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList filenames;
    filenames.append("somefile.jpg");

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames);
    Commands::CommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 1);

    QCOMPARE(artItemsBeginInsertSpy.count(), 1);
    QList<QVariant> artItemsSpyArguments = artItemsBeginInsertSpy.takeFirst();
    QCOMPARE(artItemsSpyArguments.at(1).toInt(), 0);
    QCOMPARE(artItemsSpyArguments.at(2).toInt(), 0);

    QCOMPARE(artItemsEndInsertSpy.count(), 1);
    artItemsSpyArguments = artItemsEndInsertSpy.takeFirst();
    QCOMPARE(artItemsSpyArguments.at(1).toInt(), 0);
    QCOMPARE(artItemsSpyArguments.at(2).toInt(), 0);

    QCOMPARE(artworkRepoBeginInsertSpy.count(), 1);
    QList<QVariant> artworkRepSpyArgs = artworkRepoBeginInsertSpy.takeFirst();
    QCOMPARE(artworkRepSpyArgs.at(1).toInt(), 0);
    QCOMPARE(artworkRepSpyArgs.at(2).toInt(), 0);

    QCOMPARE(artworkRepoEndInsertSpy.count(), 1);
    artworkRepSpyArgs = artworkRepoEndInsertSpy.takeFirst();
    QCOMPARE(artworkRepSpyArgs.at(1).toInt(), 0);
    QCOMPARE(artworkRepSpyArgs.at(2).toInt(), 0);
}
