#include "filteredmodel_tests.h"

void FilteredModelTests::invertSelectionTest()
{
    Mocks::CommandManagerMock commandManagerMock;
    Mocks::ArtItemsModelMock artItemsMock;

    Models::ArtworksRepository artworksRepository;
    commandManagerMock.InjectDependency(&artworksRepository);

    Models::ArtItemsModel *artItemsModel = &artItemsMock;
    commandManagerMock.InjectDependency(artItemsModel);
    int num_items=10;
    commandManagerMock.generateAndAddArtworks(num_items);


    Models::FilteredArtItemsProxyModel filteredArtItemsModel;
    filteredArtItemsModel.setSourceModel(artItemsModel);
    commandManagerMock.InjectDependency(&filteredArtItemsModel);

    // init 0 % selected
    filteredArtItemsModel.reverseFilteredArtworks();
    int selected=filteredArtItemsModel.getSelectedArtworksCount();
    int all_items=filteredArtItemsModel.getItemsCount();
    QVERIFY(selected==all_items);

    // init 100% selected
    filteredArtItemsModel.reverseFilteredArtworks();
    selected=filteredArtItemsModel.getSelectedArtworksCount();
    QVERIFY(selected==0);

    // init 50% selected
    for (int i =0; i<num_items/2; i++){
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(true);
    }
    filteredArtItemsModel.reverseFilteredArtworks();
    selected=filteredArtItemsModel.getSelectedArtworksCount();
    QVERIFY(selected==(num_items-num_items/2));

    //init 33 % selected
    for (int i =0; i<num_items; i++){                                   /* reset */
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(false);
    }
    for (int i =0; i<num_items/3; i++){                                 /* set */
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(true);
    }
    filteredArtItemsModel.reverseFilteredArtworks();
    selected=filteredArtItemsModel.getSelectedArtworksCount();
    QVERIFY(selected==(num_items-num_items/3));

    // init 1 item selected
    for (int i =0; i<num_items; i++){                                   /*reset*/
        int originalIndex = filteredArtItemsModel.getOriginalIndex(i);
        (artItemsModel->getArtwork(originalIndex))->setIsSelected(false);
    }
    int originalIndex = filteredArtItemsModel.getOriginalIndex(0);      /*set*/
    (artItemsModel->getArtwork(originalIndex))->setIsSelected(true);
    filteredArtItemsModel.reverseFilteredArtworks();
    selected=filteredArtItemsModel.getSelectedArtworksCount();
    QVERIFY(selected==(num_items-num_items/3));


}


