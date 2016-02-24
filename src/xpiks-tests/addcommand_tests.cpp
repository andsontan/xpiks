#include <QStringList>
#include <QSignalSpy>
#include "addcommand_tests.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../xpiks-qt/Commands/addartworkscommand.h"
#include "../xpiks-qt/Models/settingsmodel.h"

void AddCommand_Tests::addNoArtworksToEmptyRepositoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QSignalSpy artItemsBeginInsertSpy(&artItemsMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artItemsEndInsertSpy(&artItemsMock, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoBeginInsertSpy(&artworksRepository, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoEndInsertSpy(&artworksRepository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList filenames;

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, QStringList(), false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 0);

    QCOMPARE(artItemsBeginInsertSpy.count(), 0);
    QCOMPARE(artItemsEndInsertSpy.count(), 0);
    QCOMPARE(artworkRepoBeginInsertSpy.count(), 0);
    QCOMPARE(artworkRepoEndInsertSpy.count(), 0);
}

void AddCommand_Tests::addOneArtworkToEmptyRepositoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QSignalSpy artItemsBeginInsertSpy(&artItemsMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artItemsEndInsertSpy(&artItemsMock, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoBeginInsertSpy(&artworksRepository, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoEndInsertSpy(&artworksRepository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList filenames;
    filenames.append("somefile.jpg");

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, QStringList(), false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
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

void AddCommand_Tests::addAndAttachVectorsTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/path/to/somefile.eps" << "/another/path/to/some/other/file.eps";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, vectors, false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(artItemsModel->getArtwork(i)->hasVectorAttached());
    }
}

void AddCommand_Tests::addAndAttachVectorsLaterTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/path/to/somefile.eps" << "/another/path/to/some/other/file.eps";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, QStringList(), false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getArtwork(i)->hasVectorAttached());
    }

    addArtworksCommand = new Commands::AddArtworksCommand(QStringList(), vectors, false);
    result = commandManagerMock.processCommand(addArtworksCommand);
    addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 0);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(artItemsModel->getArtwork(i)->hasVectorAttached());
    }
}

void AddCommand_Tests::addAndDontAttachVectorsOtherDirTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/another/path/to/somefile.eps" << "/path/to/some/other/file.eps";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, vectors, false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getArtwork(i)->hasVectorAttached());
    }
}

void AddCommand_Tests::addAndDontAttachVectorsEmptyDirTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "somefile.eps" << "file.eps";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, vectors, false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getArtwork(i)->hasVectorAttached());
    }
}

void AddCommand_Tests::addAndDontAttachVectorsStartsWithTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/to/somefile.eps" << "/path/to/some/other/file.eps";

    Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, vectors, false);
    Commands::ICommandResult *result = commandManagerMock.processCommand(addArtworksCommand);
    Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
    int newFilesCount = addArtworksResult->m_NewFilesAdded;
    delete result;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getArtwork(i)->hasVectorAttached());
    }
}
