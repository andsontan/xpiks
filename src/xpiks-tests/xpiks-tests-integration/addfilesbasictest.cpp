#include "addfilesbasictest.h"
#include <QUrl>
#include <QEventLoop>
#include <QFileInfo>
#include <QStringList>
#include "integrationtestbase.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"

QString AddFilesBasicTest::testName() {
    return QLatin1String("AddFilesBasicTest");
}

void AddFilesBasicTest::setup() {
}

int AddFilesBasicTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << QUrl::fromLocalFile(QFileInfo("images-for-tests/vectors/026.jpg").absoluteFilePath());

    QEventLoop waitForArtworksAdded;
    QObject::connect(artItemsModel, SIGNAL(artworksAdded(int,int)), &waitForArtworksAdded, SLOT(quit()));

    int addedCount = artItemsModel->addLocalArtworks(files);
    waitForArtworksAdded.exec();

    VERIFY(addedCount == 1, "Failed to add file");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();

    QEventLoop waitForImportFinished;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waitForImportFinished, SLOT(quit()));

    ioCoordinator->readMetadata(true);
    waitForImportFinished.exec();

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    const QStringList &keywords = metadata->getKeywords();

    QStringList expectedKeywords = QString("abstract,art,black,blue,creative,dark,decor,decoration,decorative,design,dot,drops,elegance,element,geometric,interior,light,modern,old,ornate,paper,pattern,purple,retro,seamless,style,textile,texture,vector,wall,wallpaper").split(',');

    VERIFY(expectedKeywords == keywords, "Keywords are not the same!");

    return 0;
}

void AddFilesBasicTest::teardown() {
    m_CommandManager->cleanup();
}
