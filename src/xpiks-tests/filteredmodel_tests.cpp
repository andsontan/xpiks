#include "filteredmodel_tests.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count);

void FilteredModelTests::invertSelectionForEmptyTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, allItemsCount);
}

void FilteredModelTests::invertSelectionForAllSelectedTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i < allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(true);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, 0);
}

void FilteredModelTests::invertSelectionForSingleTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();

    artItemsModelMock.getArtwork(0)->setIsSelected(true);

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount-1));
}

void FilteredModelTests::invertSelectionForHalfSelectedTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i < allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/2);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/2));
}

void FilteredModelTests::invertSelectionForEvenCountTest(){
    DECLARE_MODELS_AND_GENERATE(10);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/3);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/3));
}

void FilteredModelTests::invertSelectionForOddCountTest(){
    DECLARE_MODELS_AND_GENERATE(11);

    int allItemsCount = filteredItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++) {
        artItemsModelMock.getArtwork(i)->setIsSelected(i < allItemsCount/3);
    }

    filteredItemsModel.invertSelectionArtworks();

    int selected = filteredItemsModel.retrieveNumberOfSelectedItems();
    QCOMPARE(selected, (allItemsCount - allItemsCount/3));
}

void FilteredModelTests::removeMetadataMarksAsModifiedTest() {
    DECLARE_MODELS_AND_GENERATE(1);

    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);

    QVERIFY(!metadata->isModified());

    metadata->setIsSelected(true);
    filteredItemsModel.removeMetadataInSelected();

    QVERIFY(metadata->isModified());
}


