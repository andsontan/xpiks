#include "zipartworkstest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/ziparchiver.h"
#include "../../xpiks-qt/Helpers/filenameshelpers.h"

QString ZipArtworksTest::testName() {
    return QLatin1String("ZipArtworksTest");
}

void ZipArtworksTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(true);
}

int ZipArtworksTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/vector/026.jpg");
    files << getImagePathForTest("images-for-tests/vector/027.jpg");

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add files");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    filteredModel->selectFilteredArtworks();
    filteredModel->setSelectedForZipping();

    Models::ZipArchiver *zipArchiver = m_CommandManager->getZipArchiver();

    QObject::connect(zipArchiver, SIGNAL(finishedProcessing()), &waiter, SIGNAL(finished()));

    zipArchiver->archiveArtworks();

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout while zipping artworks");
    }

    VERIFY(!zipArchiver->getIsError(), "Errors while zipping");

    for (int i = 0; i < files.length(); ++i) {
        Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(i);
        QString zipPath = Helpers::getArchivePath(metadata->getFilepath());

        VERIFY(QFileInfo(zipPath).exists(), "Zip file not found");
    }

    return 0;
}


