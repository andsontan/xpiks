#include "artworkmetadata_tests.h"
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include "Mocks/artworkmetadatamock.h"

void ArtworkMetadataTests::initializeOverwriteTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QString rawKeywords = "keyword1, keyword2,keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initialize(title, description, rawKeywords, true);

    // overwrite in this case is true
    QCOMPARE(result, true);

    QCOMPARE(metadata.getTitle(), title);
    QCOMPARE(metadata.getDescription(), description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword2"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword3"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initializeNoOverwriteButEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QString rawKeywords = "keyword1, keyword2,keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initialize(title, description, rawKeywords, false);

    // overwrite in this case is false, but metadata is empty
    QCOMPARE(result, true);

    QCOMPARE(metadata.getTitle(), title);
    QCOMPARE(metadata.getDescription(), description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword2"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword3"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initializeNoOverwriteNotEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    metadata.setDescription("Test Description");
    metadata.setTitle("");
    metadata.setKeywords(QStringList() << "keyword1" << "keyword3");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QString rawKeywords = "keyword1, keyword2,keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initialize(title, description, rawKeywords, false);

    // some items will be overwritten
    QCOMPARE(result, true);

    QCOMPARE(metadata.getTitle(), title);
    QVERIFY(metadata.getDescription() != description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword3"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword2"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initializeNoOverwriteTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    metadata.setDescription("Existing Description");
    metadata.setTitle("Existing Title");
    metadata.setKeywords(QStringList() << "keyword1" << "keyword3" << "keyword2" );

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QString rawKeywords = "keyword1, keyword2,keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initialize(title, description, rawKeywords, false);

    // no items will be overwritten
    QCOMPARE(result, false);

    QVERIFY(metadata.getTitle() != title);
    QVERIFY(metadata.getDescription() != description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword3"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword2"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::freshObjectTest() {
    QString filepath = "/path/to/my/file.jpg";
    Mocks::ArtworkMetadataMock metadata(filepath);

    QCOMPARE(metadata.getFilepath(), filepath);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.isDescriptionEmpty(), true);
    QCOMPARE(metadata.isTitleEmpty(), true);
    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isEmpty(), true);
    QCOMPARE(metadata.getIsSelected(), false);
}

void ArtworkMetadataTests::markModifiedShouldEmitSignalTest() {
     Mocks::ArtworkMetadataMock metadata("file.jpg");

     QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

     metadata.markModified();

     QCOMPARE(modifiedSpy.count(), 1);
     QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
     QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::modifiedIsNotMarkedModifiedAgainTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.markModified();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.markModified();

    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::setDescriptionEmitsModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setDescription("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::setTitleEmitsModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setTitle("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::addNewKeywordsEmitsModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(&metadata, SIGNAL(rowsInserted(QModelIndex,int,int)));

    int addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 2);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(addedSpy.count(), 1);
    QList<QVariant> addedArguments = addedSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 1);
}

void ArtworkMetadataTests::addExistingKeywordsDoesNotEmitModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    int addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 2);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(&metadata, SIGNAL(rowsInserted(QModelIndex,int,int)));

    addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 0);
    QCOMPARE(metadata.rowCount(), 2);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::addOneNewKeywordEmitsModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(&metadata, SIGNAL(rowsInserted(QModelIndex,int,int)));

    bool added = metadata.appendKeyword("keyword1");

    QCOMPARE(added, true);
    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(addedSpy.count(), 1);
    QList<QVariant> addedArguments = addedSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::addOneExistingKeywordDoesNotEmitModified() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    bool added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, true);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(&metadata, SIGNAL(rowsInserted(QModelIndex,int,int)));

    added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, false);
    QCOMPARE(metadata.rowCount(), 1);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::removeKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(&metadata, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    bool removed = metadata.removeKeywordAt(0);

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeLastKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(&metadata, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    bool removed = metadata.removeLastKeyword();

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeActualKeywordTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(&metadata, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    bool removed = metadata.removeKeywordAt(0);

    QCOMPARE(removed, true);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> removeArguments = removedSpy.takeFirst();
    QCOMPARE(removeArguments.at(1).toInt(), 0);
    QCOMPARE(removeArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::removeLastActualKeywordTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(&metadata, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    bool removed = metadata.removeLastKeyword();

    QCOMPARE(removed, true);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> removeArguments = removedSpy.takeFirst();
    QCOMPARE(removeArguments.at(1).toInt(), 0);
    QCOMPARE(removeArguments.at(2).toInt(), 0);
}
