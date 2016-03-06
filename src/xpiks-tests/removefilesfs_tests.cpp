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
    int itemsToDelete=2;
    QSignalSpy ArtRepFileDeleted(artworksRepository, SIGNAL(fileDeleted()));
    QSignalSpy ArtItemFileDeleted(artItemsModel, SIGNAL(fileDeleted()));

    commandManagerMock.generateAndAddArtworks(itemsToAdd);
    commandManagerMock.connectEntitiesSignalsSlots();
    artworksRepositoryMock.RemoveFilesAndEmitSignal(itemsToDelete);



    QCOMPARE(ArtRepFileDeleted.count(), 1);
    QCOMPARE(ArtItemFileDeleted.count(), 1);
    QCOMPARE (artItemsModel->getArtworksCount(), itemsToAdd-itemsToDelete);

}
