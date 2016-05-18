#include "artitemsmodel_tests.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"

#define DECLARE_MODELS_AND_GENERATE(count, withVector) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count, withVector);

void ArtItemsModelTests::removeUnavailableTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    for (int i = 0; i < count; ++i) {
        if (i%3 == 0) {
            artItemsModelMock.getArtwork(i)->setUnavailable();
        }
    }

    artItemsModelMock.removeUnavailableItems();

    for (int i = 0; i < artItemsModelMock.getArtworksCount(); ++i) {
        QVERIFY(!artItemsModelMock.getArtwork(i)->isUnavailable());
    }
}

void ArtItemsModelTests::unselectAllTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    for (int i = 0; i < count; ++i) {
        if (i%3 == 0) {
            artItemsModelMock.getArtwork(i)->setIsSelected(true);
        }
    }

    artItemsModelMock.forceUnselectAllItems();

    for (int i = 0; i < count; ++i) {
        QVERIFY(!artItemsModelMock.getArtwork(i)->isSelected());
    }
}

void ArtItemsModelTests::modificationChangesModifiedCountTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    const int index = 3;

    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 0);
    artItemsModelMock.getArtwork(index)->setModified();
    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 1);
    artItemsModelMock.getArtwork(index)->resetModified();
    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 0);
}

void ArtItemsModelTests::removeArtworkDirectorySimpleTest() {
    const int count = 11;
    DECLARE_MODELS_AND_GENERATE(count, false);

    int indexToRemove = 1;

    int firstDirCount = artworksRepository.getFilesCountForDirectory(indexToRemove);

    QCOMPARE(artItemsModelMock.getArtworksCount(), count);
    artItemsModelMock.removeArtworksDirectory(indexToRemove);
    QCOMPARE(artItemsModelMock.getArtworksCount(), count - firstDirCount);
}
