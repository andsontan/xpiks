#include "preset_tests.h"
#include "../xpiks-qt/PresetKeywords/presetkeywordsmodel.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManagerMock; \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    Models::ArtworksRepository artworksRepository; \
    Models::FilteredArtItemsProxyModel filteredItemsModel; \
    commandManagerMock.InjectDependency(&artworksRepository); \
    commandManagerMock.InjectDependency(&artItemsModelMock); \
    filteredItemsModel.setSourceModel(&artItemsModelMock); \
    commandManagerMock.InjectDependency(&filteredItemsModel); \
    commandManagerMock.generateAndAddArtworks(count); \
    KeywordsPreset::PresetKeywordsModel presetKeywordsModel; \
    commandManagerMock.InjectDependency(&presetKeywordsModel); \
    KeywordsPreset::FilteredPresetKeywordsModel filteredPresetKeywordsModel; \
    filteredPresetKeywordsModel.setSourceModel(&presetKeywordsModel);

void PresetTests::expandFromPresetTrivial()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword_0");
    }

    filteredItemsModel.replaceFromPreset(0,0,0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::expandFromPresetWithDublicates()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    filteredItemsModel.replaceFromPreset(0,0,0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::appendFromPresetTrivial()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    filteredItemsModel.appendFromPreset(0,0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_0" << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::appendFromPresetWithDublicates()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    filteredItemsModel.appendFromPreset(0,0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_0" << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}
