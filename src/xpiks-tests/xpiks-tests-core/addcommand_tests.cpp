#include <QStringList>
#include <QSignalSpy>
#include "addcommand_tests.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../../xpiks-qt/Commands/addartworkscommand.h"
#include "../../xpiks-qt/Models/settingsmodel.h"

void AddCommandTests::addNoArtworksToEmptyRepositoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QSignalSpy artItemsBeginInsertSpy(&artItemsMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artItemsEndInsertSpy(&artItemsMock, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoBeginInsertSpy(&artworksRepository, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoEndInsertSpy(&artworksRepository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList filenames;

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, QStringList(), false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 0);

    QCOMPARE(artItemsBeginInsertSpy.count(), 0);
    QCOMPARE(artItemsEndInsertSpy.count(), 0);
    QCOMPARE(artworkRepoBeginInsertSpy.count(), 0);
    QCOMPARE(artworkRepoEndInsertSpy.count(), 0);
}

void AddCommandTests::addOneArtworkToEmptyRepositoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QSignalSpy artItemsBeginInsertSpy(&artItemsMock, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artItemsEndInsertSpy(&artItemsMock, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoBeginInsertSpy(&artworksRepository, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy artworkRepoEndInsertSpy(&artworksRepository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList filenames;
    filenames.append("somefile.jpg");

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, QStringList(), false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

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

void AddCommandTests::addAndAttachVectorsTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/path/to/somefile.eps" << "/another/path/to/some/other/file.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addAndAttachVectorsLaterTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/path/to/somefile.eps" << "/another/path/to/some/other/file.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, QStringList(), false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }

    QSharedPointer<Commands::AddArtworksCommand> anotherAddArtworksCommand(new Commands::AddArtworksCommand(QStringList(), vectors, false));
    result = commandManagerMock.processCommand(anotherAddArtworksCommand);
    addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 0);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addAndDontAttachVectorsOtherDirTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/another/path/to/somefile.eps" << "/path/to/some/other/file.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addAndDontAttachVectorsEmptyDirTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "somefile.eps" << "file.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addAndDontAttachVectorsStartsWithTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile.jpg" << "/another/path/to/some/other/file.jpg";
    vectors << "/to/somefile.eps" << "/path/to/some/other/file.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 2);

    for (int i = 0; i < 2; ++i) {
        QVERIFY(!artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addAndAttachFromSingleDirectoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile1.jpg" << "/path/to/somefile2.jpg" << "/another/path/to/somefile1.jpg" << "/another/path/to/somefile2.jpg";
    vectors << "/path/to/somefile1.eps" << "/path/to/somefile2.eps" << "/another/path/to/somefile1.eps" << "/another/path/to/somefile2.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, vectors, false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, filenames.length());

    for (int i = 0; i < filenames.length(); ++i) {
        QVERIFY(artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}

void AddCommandTests::addSingleDirectoryAndAttachLaterTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Mocks::ArtItemsModelMock *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);

    QStringList filenames, vectors;
    filenames << "/path/to/somefile1.jpg" << "/path/to/somefile2.jpg" << "/another/path/to/somefile1.jpg" << "/another/path/to/somefile2.jpg";
    vectors << "/path/to/somefile1.eps" << "/path/to/somefile2.eps" << "/another/path/to/somefile1.eps" << "/another/path/to/somefile2.eps";

    QSharedPointer<Commands::AddArtworksCommand> addArtworksCommand(new Commands::AddArtworksCommand(filenames, QStringList(), false));
    auto result = commandManagerMock.processCommand(addArtworksCommand);
    auto addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    int newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, filenames.length());

    for (int i = 0; i < filenames.length(); ++i) {
        QVERIFY(!artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }

    QSharedPointer<Commands::AddArtworksCommand> anotherAddArtworksCommand(new Commands::AddArtworksCommand(QStringList(), vectors, false));
    result = commandManagerMock.processCommand(anotherAddArtworksCommand);
    addArtworksResult = result.dynamicCast<Commands::AddArtworksCommandResult>();
    newFilesCount = addArtworksResult->m_NewFilesAdded;

    QCOMPARE(newFilesCount, 0);

    for (int i = 0; i < filenames.length(); ++i) {
        QVERIFY(artItemsModel->getMockArtwork(i)->hasVectorAttached());
    }
}
