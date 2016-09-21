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
#include "../../xpiks-qt/Models/metadataelement.h"
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
    commandManagerMock.InjectDependency(&zipArchive); \
    commandManagerMock.generateAndAddArtworks(10);

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
    artworksRepositoryMock.removeFileAndEmitSignal();

// signals
    QCOMPARE(artRepositoryFileDeleted.count(), 1);
    QCOMPARE(ArtItemFileDeleted.count(), 1);
}

void RemoveFilesFsTests::removeArtworksNumberItems() {
    int itemsToAdd = 10, itemsToDelete = 5;
    DECLARE_MODELS_AND_GENERATE_(itemsToAdd);

    combinedModel.resetModel();

    std::vector<Models::MetadataElement> itemInfos;
    QVector<Models::ArtworkMetadata *> artworksList;

    for (int i = 0; i < itemsToAdd; i++) {
         Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
         itemInfos.emplace_back(metadata, i);
         artworksList.append(metadata);
    }

    combinedModel.resetModel();
    combinedModel.setArtworks(itemInfos);

    zipArchive.setArtworks(artworksList);

// delete
   commandManagerMock.mockDeletion(itemsToDelete);

// send accept
    commandManagerMock.mockAcceptDeletion();

//items
    QCOMPARE(artItemsModelMock.getArtworksCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(combinedModel.getArtworksCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(filteredItemsModel.getItemsCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(zipArchive.getItemsCount(), itemsToAdd - itemsToDelete);
}

void RemoveFilesFsTests::removeArtworksAllItems() {
    int itemsToAdd = 10, itemsToDelete = 10;
    DECLARE_MODELS_AND_GENERATE_(itemsToAdd);

    combinedModel.resetModel();
    std::vector<Models::MetadataElement> itemInfos;
    QVector<Models::ArtworkMetadata *> artworksList;

    for (int i = 0; i < itemsToAdd; i++) {
         Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
         itemInfos.emplace_back(metadata, i);
         artworksList.append(metadata);
    }

    combinedModel.resetModel();
    combinedModel.setArtworks(itemInfos);

    zipArchive.setArtworks(artworksList);

// delete
   commandManagerMock.mockDeletion(itemsToDelete);

// send accept
    commandManagerMock.mockAcceptDeletion();

//items
    QCOMPARE(artItemsModelMock.getArtworksCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(combinedModel.getArtworksCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(filteredItemsModel.getItemsCount(), itemsToAdd - itemsToDelete);
    QCOMPARE(zipArchive.getItemsCount(), itemsToAdd - itemsToDelete);
}

void RemoveFilesFsTests::removeVectorSmokeTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Mocks::ArtworksRepositoryMock artworksRepositoryMock;

    Models::ArtworksRepository *artworksRepository = &artworksRepositoryMock;
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.InjectDependency(artworksRepository);

    const int itemsToAdd = 5;
    QSignalSpy artRepositoryFileDeleted(artworksRepository, SIGNAL(filesUnavailable()));
    QSignalSpy vectorRemovedSpy(artItemsModel, SIGNAL(unavailableVectorsFound()));
    QSignalSpy artworkRemovedSpy(artItemsModel, SIGNAL(unavailableArtworksFound()));

    commandManagerMock.generateAndAddArtworks(itemsToAdd);
    commandManagerMock.connectEntitiesSignalsSlots();

    artworksRepositoryMock.removeVectorAndEmitSignal();

// signals
    QCOMPARE(artRepositoryFileDeleted.count(), 1);
    QCOMPARE(vectorRemovedSpy.count(), 1);
    QCOMPARE(artworkRemovedSpy.count(), 0);
}
