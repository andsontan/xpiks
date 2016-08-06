#include "readlegacysavedtest.h"
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

QString ReadLegacySavedTest::testName() {
    return QLatin1String("ReadLegacySavedTest");
}

void ReadLegacySavedTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(false);
}

int ReadLegacySavedTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/pixmap/img_0007.jpg");

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

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    const QStringList &keywords = metadata->getKeywords();

    QStringList expectedKeywords = QString("rock,nature,landscape,white,background,beautiful,sun,light,mountain,outdoor,top,rocky,snow,fog,horizon").split(',');

    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

    VERIFY(expectedKeywords == keywords, "Keywords are not the same!");
    VERIFY(image->getImageSize().width() == 2752, "Image width was read incorrectly");
    VERIFY(image->getImageSize().height() == 2794, "Image height was read incorrectly");

    return 0;
}
