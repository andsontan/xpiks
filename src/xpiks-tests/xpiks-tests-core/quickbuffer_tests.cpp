#include "quickbuffer_tests.h"
#include <QSignalSpy>
#include "Mocks/commandmanagermock.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/artworkmetadatamock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/QuickBuffer/quickbuffer.h"
#include "../../xpiks-qt/Models/artworkproxymodel.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Models/uimanager.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    QuickBuffer::QuickBuffer quickBuffer;\
    Models::UIManager uiManager;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&quickBuffer);\
    commandManagerMock.InjectDependency(&uiManager);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count);

void QuickBufferTests::copyArtworkToQuickBufferTest() {
    DECLARE_MODELS_AND_GENERATE(1);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    auto *artwork = artItemsModelMock.getMockArtwork(0);
    artwork->initialize(titleForQB, descriptionForQB, keywordsForQB, true);
    filteredItemsModel.copyToQuickBuffer(0);

    QCOMPARE(quickBuffer.getTitle(), titleForQB);
    QCOMPARE(quickBuffer.getDescription(), descriptionForQB);
    QCOMPARE(quickBuffer.getKeywords(), keywordsForQB);
}

void QuickBufferTests::copyProxyModelToQuickBufferTest() {
    DECLARE_MODELS_AND_GENERATE(1);
    Models::ArtworkProxyModel proxyModel;
    commandManagerMock.InjectDependency(&proxyModel);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    proxyModel.setSourceArtwork(filteredItemsModel.getArtworkMetadata(0), filteredItemsModel.getOriginalIndex(0));
    proxyModel.setTitle(titleForQB);
    proxyModel.setDescription(descriptionForQB);
    proxyModel.setKeywords(keywordsForQB);

    proxyModel.copyToQuickBuffer();

    QCOMPARE(quickBuffer.getTitle(), titleForQB);
    QCOMPARE(quickBuffer.getDescription(), descriptionForQB);
    QCOMPARE(quickBuffer.getKeywords(), keywordsForQB);
}

void QuickBufferTests::copyCombinedModelToQuickBufferTest() {
    DECLARE_MODELS_AND_GENERATE(1);
    Models::CombinedArtworksModel combinedModel;
    commandManagerMock.InjectDependency(&combinedModel);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    combinedModel.setTitle(titleForQB);
    combinedModel.setDescription(descriptionForQB);
    combinedModel.setKeywords(keywordsForQB);

    combinedModel.copyToQuickBuffer();

    QCOMPARE(quickBuffer.getTitle(), titleForQB);
    QCOMPARE(quickBuffer.getDescription(), descriptionForQB);
    QCOMPARE(quickBuffer.getKeywords(), keywordsForQB);
}

void QuickBufferTests::applyQuickBufferToArtworkTest() {
    DECLARE_MODELS_AND_GENERATE(2);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    quickBuffer.setTitle(titleForQB);
    quickBuffer.setDescription(descriptionForQB);
    quickBuffer.setKeywords(keywordsForQB);

    filteredItemsModel.registerCurrentItem(1);

    bool success = quickBuffer.copyToCurrentEditable();
    QVERIFY(success);

    auto *artwork = artItemsModelMock.getMockArtwork(1);
    QCOMPARE(artwork->getTitle(), titleForQB);
    QCOMPARE(artwork->getDescription(), descriptionForQB);
    QCOMPARE(artwork->getKeywords(), keywordsForQB);
}

void QuickBufferTests::applyQuickBufferToProxyModelTest() {
    DECLARE_MODELS_AND_GENERATE(2);
    Models::ArtworkProxyModel proxyModel;
    commandManagerMock.InjectDependency(&proxyModel);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    quickBuffer.setTitle(titleForQB);
    quickBuffer.setDescription(descriptionForQB);
    quickBuffer.setKeywords(keywordsForQB);

    proxyModel.setSourceArtwork(filteredItemsModel.getArtworkMetadata(1), filteredItemsModel.getOriginalIndex(1));
    proxyModel.registerAsCurrentItem();

    bool success = quickBuffer.copyToCurrentEditable();
    QVERIFY(success);

    QCOMPARE(proxyModel.getTitle(), titleForQB);
    QCOMPARE(proxyModel.getDescription(), descriptionForQB);
    QCOMPARE(proxyModel.getKeywords(), keywordsForQB);

    auto *artwork = artItemsModelMock.getMockArtwork(1);
    QCOMPARE(artwork->getTitle(), titleForQB);
    QCOMPARE(artwork->getDescription(), descriptionForQB);
    QCOMPARE(artwork->getKeywords(), keywordsForQB);
}

void QuickBufferTests::applyQuickBufferToCombinedModelTest() {
    DECLARE_MODELS_AND_GENERATE(2);
    Models::CombinedArtworksModel combinedModel;
    commandManagerMock.InjectDependency(&combinedModel);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    quickBuffer.setTitle(titleForQB);
    quickBuffer.setDescription(descriptionForQB);
    quickBuffer.setKeywords(keywordsForQB);

    filteredItemsModel.selectFilteredArtworks();
    filteredItemsModel.combineSelectedArtworks();

    combinedModel.registerAsCurrentItem();

    bool success = quickBuffer.copyToCurrentEditable();
    QVERIFY(success);

    QCOMPARE(combinedModel.getTitle(), titleForQB);
    QCOMPARE(combinedModel.getDescription(), descriptionForQB);
    QCOMPARE(combinedModel.getKeywords(), keywordsForQB);
}

void QuickBufferTests::cannotApplyWhenNoCurrentItemTest() {
    DECLARE_MODELS_AND_GENERATE(2);

    const QString titleForQB = "title for quick buffer";
    const QString descriptionForQB = "desc for quick buffer";
    QStringList keywordsForQB;
    keywordsForQB << "brand" << "new" << "keywords";

    auto *artwork = artItemsModelMock.getMockArtwork(1);
    const QString title = artwork->getTitle();
    const QString description = artwork->getDescription();
    QStringList keywords = artwork->getKeywords();

    quickBuffer.setTitle(titleForQB);
    quickBuffer.setDescription(descriptionForQB);
    quickBuffer.setKeywords(keywordsForQB);

    filteredItemsModel.registerCurrentItem(1);
    uiManager.clearCurrentItem();

    bool success = quickBuffer.copyToCurrentEditable();
    QVERIFY(!success);

    QCOMPARE(artwork->getTitle(), title);
    QCOMPARE(artwork->getDescription(), description);
    QCOMPARE(artwork->getKeywords(), keywords);
}
