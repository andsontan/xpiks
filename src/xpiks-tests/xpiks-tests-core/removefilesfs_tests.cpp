#include <QStringList>
#include <QPair>
#include <QTest>
#include <QSignalSpy>
#include "Mocks/commandmanagermock.h"
#include "Mocks/artworksrepositorymock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../xpiks-qt/Models/artworksrepository.h"
#include "removefilesfs_tests.h"


void RemoveFilesFs_Tests::removeArtworks() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Mocks::ArtWorksRepositoryMock artworksRepositoryMock;




    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.InjectDependency(artworksRepository);

    int itemsToAdd = 5;
    int itemsToDelete=1;
    QSignalSpy artRepositoryFileDeleted(artworksRepository, SIGNAL(fileUnavailable()));
    QSignalSpy ArtItemFileDeleted(artItemsModel, SIGNAL(filesUnavaliable()));

    commandManagerMock.generateAndAddArtworks(itemsToAdd);
    commandManagerMock.connectEntitiesSignalsSlots();
    artworksRepositoryMock.RemoveFilesAndEmitSignal(itemsToDelete);

    QCOMPARE(artRepositoryFileDeleted.count(), 1);
    QCOMPARE(ArtItemFileDeleted.count(), 1);

}
