#include "autoattachvectorstest.h"
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

QString AutoAttachVectorsTest::testName() {
    return QLatin1String("AutoAttachVectorsTest");
}

void AutoAttachVectorsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(true);
}

int AutoAttachVectorsTest::doTest() {
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

    for (int i = 0; i < files.length(); ++i) {
        Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(i);
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
        VERIFY(image != NULL && image->hasVectorAttached(), "Vector is not attached!");
    }

    return 0;
}
