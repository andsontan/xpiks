#include "artitemsmodel_tests.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"

#define DECLARE_MODELS_AND_GENERATE(count, withVector) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count, withVector);

void ArtItemsModelTests::removeUnavailableTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    for (int i = 0; i < count; ++i) {
        if (i%3 == 0) {
            artItemsModelMock.getArtwork(i)->setUnavailable();
        }
    }

    artItemsModelMock.removeUnavailableItems();

    for (int i = 0; i < artItemsModelMock.getArtworksCount(); ++i) {
        QVERIFY(!artItemsModelMock.getArtwork(i)->isUnavailable());
    }
}

void ArtItemsModelTests::unselectAllTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    for (int i = 0; i < count; ++i) {
        if (i%3 == 0) {
            artItemsModelMock.getArtwork(i)->setIsSelected(true);
        }
    }

    artItemsModelMock.forceUnselectAllItems();

    for (int i = 0; i < count; ++i) {
        QVERIFY(!artItemsModelMock.getArtwork(i)->isSelected());
    }
}

void ArtItemsModelTests::modificationChangesModifiedCountTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    const int index = 3;

    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 0);
    artItemsModelMock.getArtwork(index)->setModified();
    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 1);
    artItemsModelMock.getArtwork(index)->resetModified();
    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 0);
}

void ArtItemsModelTests::removeArtworkDirectorySimpleTest() {
    const int count = 11;
    DECLARE_MODELS_AND_GENERATE(count, false);

    int indexToRemove = 1;

    int firstDirCount = artworksRepository.getFilesCountForDirectory(indexToRemove);

    QCOMPARE(artItemsModelMock.getArtworksCount(), count);
    artItemsModelMock.removeArtworksDirectory(indexToRemove);
    QCOMPARE(artItemsModelMock.getArtworksCount(), count - firstDirCount);
}

void ArtItemsModelTests::setAllSavedResetsModifiedCountTest() {
    const int count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);
    QVector<int> selectedItems;

    for (int i = 0; i < count; ++i) {
        if (i%3 == 0) {
            artItemsModelMock.getArtwork(i)->setModified();
            selectedItems.append(i);
        }
    }

    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), selectedItems.count());

    artItemsModelMock.setSelectedItemsSaved(selectedItems);
    QCOMPARE(artItemsModelMock.getModifiedArtworksCount(), 0);
}

void ArtItemsModelTests::removingLockedArtworksTest() {
    const size_t count = 10;
    DECLARE_MODELS_AND_GENERATE(count, false);

    for (int i = 0; i < (int)count; ++i) {
        artItemsModelMock.getArtwork(i)->acquire();
    }

    QCOMPARE(artItemsModelMock.getFinalizationList().size(), (size_t)0);
    artItemsModelMock.deleteAllItems();
    QCOMPARE(artItemsModelMock.getFinalizationList().size(), count);
}

void ArtItemsModelTests::plainTextEditToEmptyKeywordsTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);
    artItemsModelMock.getMockArtwork(0)->appendKeywords(QStringList() << "test" << "keywords" << "here");

    artItemsModelMock.plainTextEdit(0, "");
    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords().length(), 0);
}

void ArtItemsModelTests::plainTextEditToOneKeywordTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);
    artItemsModelMock.getMockArtwork(0)->appendKeywords(QStringList() << "test" << "keywords" << "here");

    QString keywords = "new keyword";
    QStringList result = QStringList() << keywords;

    artItemsModelMock.plainTextEdit(0, keywords);
    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords(), result);
}

void ArtItemsModelTests::plainTextEditToSeveralKeywordsTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);
    artItemsModelMock.getMockArtwork(0)->appendKeywords(QStringList() << "test" << "keywords" << "here");

    QString keywords = "new keyword, another one, new";
    QStringList result = QStringList() << "new keyword" << "another one" << "new";

    artItemsModelMock.plainTextEdit(0, keywords);
    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords(), result);
}

void ArtItemsModelTests::plainTextEditToAlmostEmptyTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);
    artItemsModelMock.getMockArtwork(0)->appendKeywords(QStringList() << "test" << "keywords" << "here");

    QString keywords = ",,, , , , , ,,,,   ";
    QStringList result = QStringList();

    artItemsModelMock.plainTextEdit(0, keywords);
    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords(), result);
}

void ArtItemsModelTests::plainTextEditToMixedTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);
    artItemsModelMock.getMockArtwork(0)->appendKeywords(QStringList() << "test" << "keywords" << "here");

    QString keywords = ",,, , ,word here , , ,,,,   ";
    QStringList result = QStringList() << "word here";

    artItemsModelMock.plainTextEdit(0, keywords);
    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords(), result);
}
