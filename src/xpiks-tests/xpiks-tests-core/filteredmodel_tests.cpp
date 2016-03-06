#include "filteredmodel_tests.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/artworkuploader.h"

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

void FilteredModelTests::removeMetadataDeletesMetadataTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword1" << "keyword2");
        metadata->setIsSelected(true);
    }

    filteredItemsModel.removeMetadataInSelected();

    for (int i = 0; i < 10; ++i) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QVERIFY(metadata->isDescriptionEmpty());
        QVERIFY(metadata->isTitleEmpty());
        QVERIFY(metadata->areKeywordsEmpty());
        QVERIFY(metadata->isModified());
    }
}

void FilteredModelTests::findSelectedIndexTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    artItemsModelMock.getArtwork(4)->setIsSelected(true);
    int index = filteredItemsModel.findSelectedItemIndex();
    QCOMPARE(index, 4);

    artItemsModelMock.getArtwork(9)->setIsSelected(true);
    index = filteredItemsModel.findSelectedItemIndex();
    QCOMPARE(index, -1);
}

void FilteredModelTests::clearKeywordsTest() {
    DECLARE_MODELS_AND_GENERATE(1);

    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    metadata->initialize("title", "description", QStringList() << "keyword1" << "keyword2");

    filteredItemsModel.clearKeywords(0);

    QVERIFY(!metadata->isDescriptionEmpty());
    QVERIFY(!metadata->isTitleEmpty());
    QVERIFY(metadata->areKeywordsEmpty());
    QVERIFY(metadata->isModified());
}

void FilteredModelTests::detachVectorFromSelectedTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    for (int i = 0; i < 10; ++i) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword1" << "keyword2");
        metadata->attachVector(QString(metadata->getFilepath()).replace(".jpg", ".eps"));
        metadata->setIsSelected(true);
    }

    filteredItemsModel.detachVectorFromSelected();

    for (int i = 0; i < 10; ++i) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QVERIFY(!metadata->isModified());
        QVERIFY(!metadata->hasVectorAttached());
    }
}

void FilteredModelTests::setSelectedForUploadTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    Models::ArtworkUploader uploader(NULL);
    commandManagerMock.InjectDependency(&uploader);

    for (int i = 0; i < 10; ++i) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword1" << "keyword2");

        if (i % 2) {
            metadata->setIsSelected(true);
        }
    }

    filteredItemsModel.setSelectedForUpload();

    const QVector<Models::ArtworkMetadata*> &artworks = uploader.getArtworkList();
    QCOMPARE(artworks.length(), 5);
}
