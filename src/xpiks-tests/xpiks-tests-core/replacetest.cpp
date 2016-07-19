#include "replacetest.h"
#include <QString>
#include <QtAlgorithms>
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Commands/findandreplacecommand.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/previewmetadataelement.h"
#include "../../xpiks-qt/Common/flags.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManagerMock; \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    Models::ArtworksRepository artworksRepository; \
    Models::FilteredArtItemsProxyModel filteredItemsModel; \
    commandManagerMock.InjectDependency(&artworksRepository); \
    commandManagerMock.InjectDependency(&artItemsModelMock); \
    filteredItemsModel.setSourceModel(&artItemsModelMock); \
    commandManagerMock.InjectDependency(&filteredItemsModel); \
    commandManagerMock.generateAndAddArtworks(count);

void ReplaceTest::replaceTrivialTest() {
    const int itemsToGenerate = 10;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString replaceFrom = "Replace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";

    int flags = Common::SearchFlagCaseSensitive |
                Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle |
                Common::SearchFlagSearchKeywords;

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    auto artWorksInfo = filteredItemsModel.getSearchablePreviewOriginalItems(replaceFrom, flags);
    std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(
                new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags));
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(metadata->isModified());
    }
}

void ReplaceTest::noReplaceTrivialTest() {
    const int itemsToGenerate = 10;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString replaceFrom = "Noreplace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplaceMe";

    int flags = Common::SearchFlagCaseSensitive |
                Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle |
                Common::SearchFlagSearchKeywords;

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    auto artWorksInfo = filteredItemsModel.getSearchablePreviewOriginalItems(replaceFrom, flags);
    std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(
                new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags));
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(!metadata->isModified());
    }
}

void ReplaceTest::caseSensitiveTest() {
    const int itemsToGenerate = 10;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString replaceFrom = "rePLace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";

    int flags = Common::SearchFlagSearchDescription |
            Common::SearchFlagSearchTitle |
            Common::SearchFlagSearchKeywords;

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    auto artWorksInfo = filteredItemsModel.getSearchablePreviewOriginalItems(replaceFrom, flags);
    std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(
                new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags));
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(metadata->isModified());
    }
}

void ReplaceTest::replaceTitleTest() {
    const int itemsToGenerate = 10;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString replaceFrom = "Replace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";

    int flags = Common::SearchFlagCaseSensitive | Common::SearchFlagSearchTitle;

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    auto artWorksInfo = filteredItemsModel.getSearchablePreviewOriginalItems(replaceFrom, flags);
    std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(
                new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags));
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), initString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], initString);
        QVERIFY(metadata->isModified());
    }
}


void ReplaceTest::replaceKeywordsTest() {
    const int itemsToGenerate = 10;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString replaceFrom = "keywordOld";
    QString replaceTo = "keywordNew";
    QString replaceToLower = replaceTo.toLower();

    int flags = Common::SearchFlagCaseSensitive |
                Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle |
                Common::SearchFlagSearchKeywords;

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(QString("title"), QString("description"), QStringList() << replaceToLower<<"dummyKey"<<replaceFrom);
    }

    auto artWorksInfo = filteredItemsModel.getSearchablePreviewOriginalItems(replaceFrom, flags);
    std::shared_ptr<Commands::FindAndReplaceCommand> replaceCommand(
                new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags));
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < itemsToGenerate; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), QString("description"));
        QCOMPARE(metadata->getTitle(), QString("title"));
        QStringList test = metadata->getKeywords();
        QStringList gold;
        gold<<replaceToLower<<"dummyKey";
        qSort(gold.begin(),gold.end());
        qSort(test.begin(),test.end());
        QCOMPARE(gold, test);
        QVERIFY(metadata->isModified());
    }
}

