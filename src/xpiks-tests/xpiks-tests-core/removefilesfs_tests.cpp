#include <QStringList>
#include <QPair>
#include <QTest>
#include <QSignalSpy>
#include "Mocks/commandmanagermock.h"
#include "Mocks/artworksrepositorymock.h"
#include "Mocks/artitemsmodelmock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/artworkuploader.h"
#include "../../xpiks-qt/Models/artiteminfo.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Models/ziparchiver.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "removefilesfs_tests.h"

#define DECLARE_MODELS_AND_GENERATE_(count) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    Models::CombinedArtworksModel combinedModel; \
    commandManagerMock.InjectDependency(&combinedModel);


void RemoveFilesFs_Tests::removeArtworksSignals() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Mocks::ArtWorksRepositoryMock artworksRepositoryMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.InjectDependency(artworksRepository);

    int itemsToAdd = 5;
    QSignalSpy artRepositoryFileDeleted(artworksRepository, SIGNAL(fileUnavailable()));
    QSignalSpy ArtItemFileDeleted(artItemsModel, SIGNAL(filesUnavaliable()));

    commandManagerMock.generateAndAddArtworks(itemsToAdd);
    commandManagerMock.connectEntitiesSignalsSlots();
    artworksRepositoryMock.publicRemoveFileAndEmitSignal();
// signals
    QCOMPARE(artRepositoryFileDeleted.count(), 1);
    QCOMPARE(ArtItemFileDeleted.count(), 1);
}

void RemoveFilesFs_Tests::removeArtworksNumberItems(){

    int itemsToAdd = 10, itemsToDelete = 5;
    DECLARE_MODELS_AND_GENERATE_(itemsToAdd);
    combinedModel.resetModelData();
    QVector<Models::ArtItemInfo *> items;
    for (int i =0; i< itemsToAdd; i++){
         items.append(new Models::ArtItemInfo(commandManagerMock.getArtItemsModel()->getArtwork(i), i));
    }
    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();
   return;


// delete
   commandManagerMock.mockDeletion(itemsToDelete);

// send accept
    commandManagerMock.mockAcceptDeletion();
//items
    QCOMPARE(artItemsModelMock.getArtworksCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(combinedModel.getArtworksCount(), itemsToAdd-itemsToDelete);
}
