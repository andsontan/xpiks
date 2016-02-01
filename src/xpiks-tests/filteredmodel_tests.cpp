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

    int all_items=filteredArtItemsModel.getItemsCount();
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(all_items));
    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(false);
    }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==all_items);
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

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(true);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                   /*set*/
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

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected((i<1));
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(all_items-1));

}



void FilteredModelTests::invertSelectionForHalfSelectedTest(){
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

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){ /* set */
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(i<all_items/2);
    }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(all_items-all_items/2));
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

    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                 /* set */
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
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
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(all_items-all_items/3));
    int all_items=filteredArtItemsModel.getItemsCount();
    for (int i =0; i<all_items; i++){                                   /*set*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(i<all_items/2);
     }
    filteredArtItemsModel.invertSelectionArtworks();
    int selected=filteredArtItemsModel.retrieveNumberOfSelectedItems();
    QVERIFY(selected==(all_items-all_items/2));
}
