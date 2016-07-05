#include "combinedmodel_tests.h"
#include <QStringList>
#include <QVector>
#include <vector>
#include <QSignalSpy>
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/metadataelement.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Models/settingsmodel.h"

Models::MetadataElement createArtworkMetadata(const QString &desc, const QString &title, const QStringList &keywords, int index=0) {
    Mocks::ArtworkMetadataMock *metadata = new Mocks::ArtworkMetadataMock("/random/file/path.jpg");
    metadata->appendKeywords(keywords);
    metadata->setTitle(title);
    metadata->setDescription(desc);
    return Models::MetadataElement(metadata, index);
}

void freeArtworks(std::vector<Models::MetadataElement> &items) {
    for (auto &item: items) {
        item.freeMetadata();
    }
}

void CombinedModelTests::initTestCase() {
    m_SettingsModel.setUseSpellCheck(false);
    m_CommandManagerMock.InjectDependency(&m_SettingsModel);
    m_CommandManagerMock.disableCommands();
}

void CombinedModelTests::trivialCombineNoItemsTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    combinedModel.resetModelData();
    combinedModel.initArtworks(std::vector<Models::MetadataElement>());
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), (size_t)0);
    QVERIFY(combinedModel.getDescription().isEmpty());
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    QCOMPARE(combinedModel.getChangeDescription(), true);
    QCOMPARE(combinedModel.getChangeTitle(), true);
    QCOMPARE(combinedModel.getChangeKeywords(), true);
}

void CombinedModelTests::trivialcombineOneItemTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString desc = "Description for the item";
    QString title = "Item title";
    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(desc, title, keywords));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), (size_t)1);
    QCOMPARE(combinedModel.getDescription(), desc);
    QCOMPARE(combinedModel.getTitle(), title);
    QCOMPARE(combinedModel.getKeywordsCount(), keywords.count());
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::combineSeveralSameItemsTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString desc = "Description for the item";
    QString title = "Item title";
    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    std::vector<Models::MetadataElement> items;
    const size_t itemsToGenerate = 5;
    int numberOfItems = itemsToGenerate;
    while (numberOfItems--) {
        items.push_back(createArtworkMetadata(desc, title, keywords));
    }

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), itemsToGenerate);
    QCOMPARE(combinedModel.getDescription(), desc);
    QCOMPARE(combinedModel.getTitle(), title);
    QCOMPARE(combinedModel.getKeywordsCount(), keywords.count());
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::combineAllDifferentItemsTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3", 2));

    size_t size = items.size();

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size);
    QVERIFY(combinedModel.getDescription().isEmpty());
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::combineCommonInKeywordsTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonKeyword = "a common keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << commonKeyword, 2));

    size_t size = items.size();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size);
    QVERIFY(combinedModel.getDescription().isEmpty());
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getKeywordsCount(), 1);
    QCOMPARE(combinedModel.getKeywords()[0], commonKeyword);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::combineCommonInTitleTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonTitle = "a common title";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", commonTitle, QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", commonTitle, QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", commonTitle, QStringList() << "Keyword3", 2));

    size_t size = items.size();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size);
    QVERIFY(combinedModel.getDescription().isEmpty());
    QCOMPARE(combinedModel.getTitle(), commonTitle);
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::combineCommonInDescriptionTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata(commonDescription, "title3", QStringList() << "Keyword3", 2));

    size_t size = items.size();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size);
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getDescription(), commonDescription);
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::recombineAfterRemoveDifferentTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonDescription = "a common Description1";
    QString commonKeyword = "keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Different description", "title3", QStringList() << "Keyword3", 2));

    size_t size = items.size();

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.getItems().back().setSelected(true);
    combinedModel.removeSelectedArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size - 1);
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getDescription(), commonDescription);
    QCOMPARE(combinedModel.getKeywordsCount(), 1);
    QCOMPARE(combinedModel.getKeywords()[0], commonKeyword);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::recombineAfterRemoveAllButOneTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << "adfafdaf", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << "21345425421", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << "(*&^*&^*&&^%", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.getItems()[1].setSelected(true);
    combinedModel.getItems()[2].setSelected(true);
    combinedModel.removeSelectedArtworks();
    Models::ArtworkMetadata *first = combinedModel.getItems().front().getOrigin();

    QCOMPARE(combinedModel.getArtworksCount(), (size_t)1);
    QCOMPARE(combinedModel.getTitle(), first->getTitle());
    QCOMPARE(combinedModel.getDescription(), first->getDescription());
    QCOMPARE(combinedModel.getKeywordsCount(), first->getKeywordsModel()->getKeywordsCount());
    QCOMPARE(combinedModel.getKeywords(), first->getKeywords());
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::recombineAfterChangesTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << "adfafdaf", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << "21345425421", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << "(*&^*&^*&&^%", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.appendKeyword("brand new keyword");
    combinedModel.setDescription(combinedModel.getDescription() + " new stuff here");
    combinedModel.setTitle(combinedModel.getTitle() + " new stuff here");

    combinedModel.getItems()[1].setSelected(true);
    combinedModel.getItems()[2].setSelected(true);
    combinedModel.removeSelectedArtworks();
    Models::ArtworkMetadata *first = combinedModel.getItems().front().getOrigin();

    QCOMPARE(combinedModel.getArtworksCount(), (size_t)1);
    QVERIFY(combinedModel.getTitle() != first->getTitle());
    QVERIFY(combinedModel.getDescription() != first->getDescription());
    QVERIFY(combinedModel.getKeywordsCount() != first->getKeywordsModel()->getKeywordsCount());
    QVERIFY(combinedModel.getKeywords() != first->getKeywords());
    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::twoTimesInARowRecombineTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonDescription = "a common Description1";
    QString commonKeyword = "keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Different description", "title3", QStringList() << "Keyword3", 2));

    size_t size = items.size();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getArtworksCount(), size);
    QVERIFY(combinedModel.getTitle().isEmpty());
    QVERIFY(combinedModel.getDescription().isEmpty());
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
}

void CombinedModelTests::isNotModifiedAfterTitleDescEditTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata(commonDescription, "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.setDescription("Brand new description");
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.setTitle("Brand new title");
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::isModifiedAfterKeywordsAppendTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata(commonDescription, "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.appendKeyword("Brand new keyword");

    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::isModifiedAfterKeywordRemovalTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonKeyword = "a common keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << commonKeyword, 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.removeLastKeyword();

    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::isModifiedAfterKeywordEditTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonKeyword = "a common keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << commonKeyword, 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.editKeyword(0, "another keyword");

    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::isModifiedAfterKeywordsClearTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonKeyword = "a common keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << commonKeyword, 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.clearKeywords();

    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::isNotModifiedAfterEmptyKeywordsClearTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.clearKeywords();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    freeArtworks(items);
}

void CombinedModelTests::isModifiedStatusNotResetWithOtherTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    combinedModel.appendKeyword("new keyword");
    combinedModel.removeLastKeyword();
    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    combinedModel.clearKeywords();

    QCOMPARE(combinedModel.getAreKeywordsModified(), true);

    freeArtworks(items);
}

void CombinedModelTests::initArtworksEmitsRowsInsertTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString commonKeyword = "a common keyword";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1" << commonKeyword, 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2" << commonKeyword, 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3" << commonKeyword, 2));

    QSignalSpy resetSpy(&combinedModel, SIGNAL(modelReset()));

    combinedModel.initArtworks(items);

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    QCOMPARE(resetSpy.count(), 1);

    freeArtworks(items);
}

void CombinedModelTests::initEmptyArtworksDoesNotEmitTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QSignalSpy addSpy(&combinedModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    combinedModel.initArtworks(std::vector<Models::MetadataElement>());

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    QCOMPARE(addSpy.count(), 0);
    QCOMPARE(combinedModel.getChangeDescription(), false);
    QCOMPARE(combinedModel.getChangeTitle(), false);
    QCOMPARE(combinedModel.getChangeKeywords(), false);
}

void CombinedModelTests::initOneArtworkEnablesAllFields() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));

    combinedModel.initArtworks(items);

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    QCOMPARE(combinedModel.getChangeDescription(), true);
    QCOMPARE(combinedModel.getChangeTitle(), true);
    QCOMPARE(combinedModel.getChangeKeywords(), true);

    freeArtworks(items);
}

void CombinedModelTests::initManyArtworksDoesNotEnableAllFields() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    QCOMPARE(combinedModel.getChangeDescription(), false);
    QCOMPARE(combinedModel.getChangeTitle(), false);
    QCOMPARE(combinedModel.getChangeKeywords(), false);

    freeArtworks(items);
}

void CombinedModelTests::resetModelClearsEverythingTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata("Description1", "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata("Description2", "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata("Description3", "title3", QStringList() << "Keyword3", 2));

    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.resetModelData();

    QCOMPARE(combinedModel.getArtworksCount(), (size_t)0);
    QVERIFY(combinedModel.getDescription().isEmpty());
    QVERIFY(combinedModel.getTitle().isEmpty());
    QCOMPARE(combinedModel.getKeywordsCount(), 0);
    QCOMPARE(combinedModel.getAreKeywordsModified(), false);
    // TEMPORARY
    QCOMPARE(combinedModel.getChangeDescription(), true);
    QCOMPARE(combinedModel.getChangeTitle(), true);
    QCOMPARE(combinedModel.getChangeKeywords(), true);
}

void CombinedModelTests::appendNewKeywordEmitsCountChangedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QSignalSpy addSpy(&combinedModel, SIGNAL(keywordsCountChanged()));

    combinedModel.appendKeyword("new keyword");

    QCOMPARE(addSpy.count(), 1);
}

void CombinedModelTests::appendExistingKeywordDoesNotEmitTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString keyword = "new keyword";

    combinedModel.appendKeyword(keyword);

    QSignalSpy addSpy(&combinedModel, SIGNAL(keywordsCountChanged()));

    combinedModel.appendKeyword(keyword);

    QCOMPARE(addSpy.count(), 0);
}

void CombinedModelTests::pasteNewKeywordsEmitsCountChangedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QSignalSpy addSpy(&combinedModel, SIGNAL(keywordsCountChanged()));

    combinedModel.pasteKeywords(QStringList() << "new keyword" << "another keyword");

    QCOMPARE(addSpy.count(), 1);
}

void CombinedModelTests::pasteExistingKeywordsDoesNotEmitTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QStringList keywords;
    keywords << "new keyword" << "another keyword";

    combinedModel.pasteKeywords(keywords);

    QSignalSpy addSpy(&combinedModel, SIGNAL(keywordsCountChanged()));
    combinedModel.pasteKeywords(keywords);

    QCOMPARE(addSpy.count(), 0);
}

void CombinedModelTests::editKeywordDoesNotEmitCountChangedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);

    QString keyword = "new keyword";

    combinedModel.appendKeyword(keyword);

    QSignalSpy addSpy(&combinedModel, SIGNAL(keywordsCountChanged()));

    combinedModel.editKeyword(0, "another");

    QCOMPARE(addSpy.count(), 0);
}

void CombinedModelTests::notSavedAfterAllDisabledTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1" << "keyword2", 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    QCOMPARE(combinedModel.getAreKeywordsModified(), false);

    combinedModel.setDescription("Brand new description");
    combinedModel.setTitle("Brand new title");
    combinedModel.appendKeyword("brand new keyword");

    combinedModel.setChangeDescription(false);
    combinedModel.setChangeTitle(false);
    combinedModel.setChangeKeywords(false);

    combinedModel.saveEdits();

    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), false);

    freeArtworks(items);
}

void CombinedModelTests::notSavedAfterNothingModifiedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.saveEdits();

    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), false);

    freeArtworks(items);
}

void CombinedModelTests::notSavedAfterModifiedDisabledTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.setDescription("Brand new description");
    combinedModel.setChangeDescription(false);
    combinedModel.saveEdits();
    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), false);

    combinedModel.setTitle("Brand new title");
    combinedModel.setChangeTitle(false);
    combinedModel.saveEdits();
    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), false);

    freeArtworks(items);
}

void CombinedModelTests::savedAfterModifiedDescriptionTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.setDescription("Brand new description");
    combinedModel.saveEdits();
    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), true);

    freeArtworks(items);
}

void CombinedModelTests::savedAfterModifiedTitleTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.setTitle("Brand new title");
    combinedModel.saveEdits();
    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), true);

    freeArtworks(items);
}

void CombinedModelTests::savedAfterKeywordsModifiedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.appendKeyword("Brand new keyword");
    combinedModel.saveEdits();
    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), true);

    freeArtworks(items);
}

void CombinedModelTests::savedIfMoreThanOneButNotModifiedTest() {
    Models::CombinedArtworksModel combinedModel;
    combinedModel.setCommandManager(&m_CommandManagerMock);
    m_CommandManagerMock.resetAnyCommandProcessed();

    QString commonDescription = "a common Description1";

    std::vector<Models::MetadataElement> items;
    items.push_back(createArtworkMetadata(commonDescription, "title1", QStringList() << "Keyword1", 0));
    items.push_back(createArtworkMetadata(commonDescription, "title2", QStringList() << "Keyword2", 1));
    items.push_back(createArtworkMetadata(commonDescription, "title3", QStringList() << "Keyword3", 2));

    combinedModel.resetModelData();
    combinedModel.initArtworks(items);
    combinedModel.recombineArtworks();

    combinedModel.saveEdits();

    QCOMPARE(m_CommandManagerMock.anyCommandProcessed(), true);

    freeArtworks(items);
}
