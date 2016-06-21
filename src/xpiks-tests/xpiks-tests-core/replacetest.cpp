#include "replacetest.h"
#include "Mocks/artitemsmodelmock.h"
#include "../../xpiks-qt/Commands/findandreplacecommand.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Common/flags.h"
#include <QtTest/QTest>
#include <QString>

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
    DECLARE_MODELS_AND_GENERATE(10);
    QString replaceFrom = "Replace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";

    int flags = Common::SearchFlagCaseSensitive |
                Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle |
                Common::SearchFlagSearchKeywords;

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() <<initString);
    }

    QVector<Models::ArtItemInfo *> artWorksInfo = filteredItemsModel.getSearchableOriginalItemsWithIndices(replaceFrom, flags);
    QSharedPointer<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags) );
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(artItemsModelMock.getArtwork(i)->isModified());
    }
}

void ReplaceTest::noReplaceTrivialTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    QString replaceFrom = "Noreplace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplaceMe";
    int flags = Common::SearchFlagCaseSensitive |
                Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle |
                Common::SearchFlagSearchKeywords;

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    QVector<Models::ArtItemInfo *> artWorksInfo = filteredItemsModel.getSearchableOriginalItemsWithIndices(replaceFrom, flags);
    QSharedPointer<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags) );
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(!artItemsModelMock.getArtwork(i)->isModified());
    }
}

void ReplaceTest::caseSensitiveTest() {
    DECLARE_MODELS_AND_GENERATE(10);
    QString replaceFrom = "rePLace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";
    int flags = Common::SearchFlagSearchDescription |
                Common::SearchFlagSearchTitle | Common::SearchFlagSearchKeywords;

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }

    QVector<Models::ArtItemInfo *> artWorksInfo = filteredItemsModel.getSearchableOriginalItemsWithIndices(replaceFrom, flags);
    QSharedPointer<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags) );
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), finalString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], finalString);
        QVERIFY(artItemsModelMock.getArtwork(i)->isModified());
    }
}

void ReplaceTest::categoryTest() {
    DECLARE_MODELS_AND_GENERATE(10);

    QString replaceFrom = "Replace";
    QString replaceTo = "Replaced";
    QString initString = "ReplaceMe";
    QString finalString = "ReplacedMe";
    int flags = Common::SearchFlagCaseSensitive |Common::SearchFlagSearchTitle;

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        metadata->initialize(initString, initString, QStringList() << initString);
    }
    QVector<Models::ArtItemInfo *> artWorksInfo = filteredItemsModel.getSearchableOriginalItemsWithIndices(replaceFrom, flags);
    QSharedPointer<Commands::FindAndReplaceCommand> replaceCommand(new Commands::FindAndReplaceCommand(artWorksInfo, replaceFrom, replaceTo, flags) );
    auto result = commandManagerMock.processCommand(replaceCommand);

    for (int i = 0; i < 10; i++) {
        Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(i);
        QCOMPARE(metadata->getDescription(), initString);
        QCOMPARE(metadata->getTitle(), finalString);
        QCOMPARE(metadata->getKeywords()[0], initString);
        QVERIFY(artItemsModelMock.getArtwork(i)->isModified());
    }
}
