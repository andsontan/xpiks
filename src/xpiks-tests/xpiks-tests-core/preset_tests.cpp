#include "preset_tests.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h"
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
    KeywordsPresets::PresetKeywordsModel presetKeywordsModel; \
    commandManagerMock.InjectDependency(&presetKeywordsModel); \
    KeywordsPresets::FilteredPresetKeywordsModel filteredPresetKeywordsModel; \
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

    artItemsModelMock.expandPreset(0, 0, 0);
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

    artItemsModelMock.expandPreset(0, 0, 0);
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

    artItemsModelMock.addPreset(0, 0);
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

    artItemsModelMock.addPreset(0, 0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_0" << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::findPresetByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem("man", QStringList() << "some" << "keywords");
    presetKeywordsModel.addItem("woman", QStringList() << "other" << "keywords");

    int index;

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("man", false, index)); QCOMPARE(index, 0);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("woman", false, index)); QCOMPARE(index, 1);

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("mAn", false, index)); QCOMPARE(index, 0);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("WomaN", false, index)); QCOMPARE(index, 1);

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("an", false, index));
}

void PresetTests::strictFindPresetByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem("Man", QStringList() << "some" << "keywords");
    presetKeywordsModel.addItem("Woman", QStringList() << "other" << "keywords");

    int index;

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("man", true, index));
    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", true, index));

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Man", true, index)); QCOMPARE(index, 0);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Woman", true, index)); QCOMPARE(index, 1);
}

void PresetTests::findPresetWithLongNamesByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem("young woman", QStringList() << "some" << "keywords");
    presetKeywordsModel.addItem("old woman", QStringList() << "other" << "keywords");

    int index;

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", false, index));
    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", true, index));

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("young", false, index)); QCOMPARE(index, 0);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("old", false, index)); QCOMPARE(index, 1);

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Young woman", false, index)); QCOMPARE(index, 0);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("old Woman", false, index)); QCOMPARE(index, 1);
}
