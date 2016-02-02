#include "filteredmodel_tests.h"


void FilteredModelTests::invertSelectionForEmptyTest(){
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.generateAndAddArtworks(10);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int allItemsCount=filteredArtItemsModel.getItemsCount();
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(allItemsCount));

}

void FilteredModelTests::invertSelectionForAllSelectedTest(){
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.generateAndAddArtworks(10);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int allItemsCount=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(true);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==0);
}

void FilteredModelTests::invertSelectionForSingleTest(){
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.generateAndAddArtworks(10);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int allItemsCount=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(i==0);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(allItemsCount-1));
}
void FilteredModelTests::invertSelectionForThirdSelectedTest(){
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.generateAndAddArtworks(10);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int allItemsCount=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(i<allItemsCount/3);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(allItemsCount-allItemsCount/3));
}

void FilteredModelTests::invertSelectionForHalfSelectedTest(){
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;
    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);
    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    commandManagerMock.generateAndAddArtworks(10);
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int allItemsCount=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<allItemsCount; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(i<allItemsCount/2);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(allItemsCount-allItemsCount/2));
}

