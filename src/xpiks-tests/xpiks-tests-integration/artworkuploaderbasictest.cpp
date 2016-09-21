#include "artworkuploaderbasictest.h"
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
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworkuploader.h"

QString ArtworkUploaderBasicTest::testName() {
    return QLatin1String("ArtworkUploaderBasicTest");
}

void ArtworkUploaderBasicTest::setup() {
}

int ArtworkUploaderBasicTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/mixed/026.jpg");
    files << getImagePathForTest("images-for-tests/mixed/027.jpg");
    files << getImagePathForTest("images-for-tests/mixed/0267.jpg");

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add file");

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
    filteredModel->setSelectedForUpload();

    Models::ArtworkUploader *artworkUploader = m_CommandManager->getArtworkUploader();

    VERIFY(artworkUploader->getItemsCount() == addedCount, "Artworks didn't get to the uploader");

    return 0;
}
