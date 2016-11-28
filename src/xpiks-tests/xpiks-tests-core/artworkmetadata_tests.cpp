#include "artworkmetadata_tests.h"
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include "Mocks/artworkmetadatamock.h"

void ArtworkMetadataTests::initializeOverwriteTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

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
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

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
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";
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
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

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

void ArtworkMetadataTests::initializeOverwriteWithEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    metadata.setDescription("Existing Description");
    metadata.setTitle("Existing Title");
    metadata.setKeywords(QStringList() << "keyword1" << "keyword3" << "keyword2" );

    QString title;
    QString description;
    QStringList rawKeywords;

    bool result = metadata.initialize(title, description, rawKeywords, true);

    QCOMPARE(result, true);

    QVERIFY(metadata.isTitleEmpty());
    QVERIFY(metadata.isDescriptionEmpty());
    QVERIFY(metadata.areKeywordsEmpty());

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
    QCOMPARE(metadata.isSelected(), false);
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

void ArtworkMetadataTests::setDescriptionEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setDescription("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::setTitleEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setTitle("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::addNewKeywordsEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

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

void ArtworkMetadataTests::addExistingKeywordsDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    int addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 2);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 0);
    QCOMPARE(metadata.rowCount(), 2);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::addOneNewKeywordEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

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

void ArtworkMetadataTests::addOneExistingKeywordDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    bool added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, true);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, false);
    QCOMPARE(metadata.rowCount(), 1);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::removeKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString keyword;
    bool removed = metadata.removeKeywordAt(0, keyword);

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeLastKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString keyword;
    bool removed = metadata.removeLastKeyword(keyword);

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeActualKeywordTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    QString keyword;
    bool removed = metadata.removeKeywordAt(0, keyword);

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
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    QString keyword;
    bool removed = metadata.removeLastKeyword(keyword);

    QCOMPARE(removed, true);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> removeArguments = removedSpy.takeFirst();
    QCOMPARE(removeArguments.at(1).toInt(), 0);
    QCOMPARE(removeArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::editKeywordToAnotherEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    bool editResult = metadata.editKeyword(0, "another");

    QCOMPARE(editResult, true);
    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::editKeywordToExistingDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    bool editResult = metadata.editKeyword(0, "keyword2");

    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::misEditOfKeywordDoesNothingTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    bool editResult = metadata.editKeyword(2, "keyword2");

    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::isInDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString alsoDir = "C:\\path\\to\\directory\\of";
#else
    QString dir = "/path/to/directory/of/";
    QString alsoDir = "/path/to/directory/of";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    qDebug() << QDir(dir).absolutePath();
    QVERIFY(metadata.isInDirectory(QDir(dir).absolutePath()));
    QVERIFY(metadata.isInDirectory(QDir(alsoDir).absolutePath()));
}

void ArtworkMetadataTests::isNotInParentsDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString notADir1 = "C:\\path\\to\\directory";
    QString notADir2 = "C:\\path\\to\\directory\\";
    QString notADir3 = "C:\\path\\to\\";
#else
    QString dir = "/path/to/directory/of/";
    QString notADir1 = "/path/to/directory/";
    QString notADir2 = "/path/to/directory";
    QString notADir3 = "/path/to/";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());

    QVERIFY(!metadata.isInDirectory(QDir(notADir1).absolutePath()));
    QVERIFY(!metadata.isInDirectory(QDir(notADir2).absolutePath()));
    QVERIFY(!metadata.isInDirectory(QDir(notADir3).absolutePath()));
}

void ArtworkMetadataTests::isNotInOtherDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString otherDir = "C:\\path\\to\\some\\other\\directory\\of\\";
#else
    QString dir = "/path/to/directory/of/";
    QString otherDir = "/path/to/some/other/directory/of/";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    QVERIFY(!metadata.isInDirectory(QDir(otherDir).absolutePath()));
}

void ArtworkMetadataTests::isNotInEmptyDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString otherDir = "";
#else
    QString dir = "/path/to/directory/of/";
    QString otherDir = "";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    QVERIFY(!metadata.isInDirectory(QDir(otherDir).absolutePath()));
}

void ArtworkMetadataTests::clearKeywordsMarksAsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    metadata.resetModified();

    metadata.clearKeywords();
    QVERIFY(metadata.isModified());
}

void ArtworkMetadataTests::clearEmptyKeywordsDoesNotMarkModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QVERIFY(!metadata.isModified());

    metadata.clearKeywords();
    QVERIFY(!metadata.isModified());
}

void ArtworkMetadataTests::removeKeywordsMarksModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    metadata.resetModified();

    bool result = metadata.removeKeywords(QSet<QString>() << "keyword3");
    QVERIFY(!result);
    QVERIFY(!metadata.isModified());

    result = metadata.removeKeywords(QSet<QString>() << "keyword2");
    QVERIFY(result);
    QVERIFY(metadata.isModified());
}
