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
#include "../../xpiks-qt/Models/artworkuploader.h"
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
    commandManagerMock.InjectDependency(&combinedModel);\
    Models::ZipArchiver zipArchive; \
    commandManagerMock.InjectDependency(&zipArchive);\
    Models::ArtworkUploader artworkUpload(NULL); \
    commandManagerMock.InjectDependency(&artworkUpload);\
    commandManagerMock.generateAndAddArtworks(count);


void RemoveFilesFsTests::removeArtworksSignals() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Mocks::ArtworksRepositoryMock artworksRepositoryMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.InjectDependency(artworksRepository);

    int itemsToAdd = 5;
    QSignalSpy artRepositoryFileDeleted(artworksRepository, SIGNAL(filesUnavailable()));
    QSignalSpy ArtItemFileDeleted(artItemsModel, SIGNAL(unavailableArtworksFound()));

    commandManagerMock.generateAndAddArtworks(itemsToAdd);
    commandManagerMock.connectEntitiesSignalsSlots();
    artworksRepositoryMock.publicRemoveFileAndEmitSignal();
// signals
    QCOMPARE(artRepositoryFileDeleted.count(), 1);
    QCOMPARE(ArtItemFileDeleted.count(), 1);
}

void RemoveFilesFsTests::removeArtworksNumberItems() {

    int itemsToAdd = 10, itemsToDelete = 5;
    DECLARE_MODELS_AND_GENERATE_(itemsToAdd);
    combinedModel.resetModelData();
    QVector<Models::ArtItemInfo *> items;
    QVector<Models::ArtworkMetadata *> items_mod;
    for (int i =0; i< itemsToAdd; i++) {
         Models::ArtworkMetadata * metadata =  commandManagerMock.getArtItemsModel()->getArtwork(i);
         items.append(new Models::ArtItemInfo(metadata, i));
         items_mod.append(metadata);
    }
    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    zipArchive.setArtworks(items_mod);

    artworkUpload.setArtworks(items_mod);



// delete
   commandManagerMock.mockDeletion(itemsToDelete);

// send accept
    commandManagerMock.mockAcceptDeletion();
//items
    QCOMPARE(artItemsModelMock.getArtworksCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(combinedModel.getArtworksCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(filteredItemsModel.getItemsCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(zipArchive.getItemsCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(artworkUpload.getItemsCount(), itemsToAdd-itemsToDelete);
}


void RemoveFilesFsTests::removeArtworksAllItems() {

    int itemsToAdd = 10, itemsToDelete = 10;
    DECLARE_MODELS_AND_GENERATE_(itemsToAdd);
    combinedModel.resetModelData();
    QVector<Models::ArtItemInfo *> items;
    QVector<Models::ArtworkMetadata *> items_mod;
    for (int i =0; i< itemsToAdd; i++) {
         Models::ArtworkMetadata * metadata =  commandManagerMock.getArtItemsModel()->getArtwork(i);
         items.append(new Models::ArtItemInfo(metadata, i));
         items_mod.append(metadata);
    }
    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    zipArchive.setArtworks(items_mod);

    artworkUpload.setArtworks(items_mod);



// delete
   commandManagerMock.mockDeletion(itemsToDelete);

// send accept
    commandManagerMock.mockAcceptDeletion();
//items
    QCOMPARE(artItemsModelMock.getArtworksCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(combinedModel.getArtworksCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(filteredItemsModel.getItemsCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(zipArchive.getItemsCount(), itemsToAdd-itemsToDelete);
    QCOMPARE(artworkUpload.getItemsCount(), itemsToAdd-itemsToDelete);
}
