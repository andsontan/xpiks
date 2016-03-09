#include "addfilesbasictest.h"
#include <QUrl>
#include <QEventLoop>
#include <QFileInfo>
#include <QStringList>
#include <QTimer>
#include "integrationtestbase.h"
#include "signalwaiter.h"
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
    files << QUrl::fromLocalFile(QFileInfo("images-for-tests/vector/026.jpg").absoluteFilePath());

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == 1, "Failed to add file");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    const QStringList &keywords = metadata->getKeywords();

    QStringList expectedKeywords = QString("abstract,art,black,blue,creative,dark,decor,decoration,decorative,design,dot,drops,elegance,element,geometric,interior,light,modern,old,ornate,paper,pattern,purple,retro,seamless,style,textile,texture,vector,wall,wallpaper").split(',');

    VERIFY(expectedKeywords == keywords, "Keywords are not the same!");

    return 0;
}

void AddFilesBasicTest::teardown() {
    m_CommandManager->cleanup();
}
