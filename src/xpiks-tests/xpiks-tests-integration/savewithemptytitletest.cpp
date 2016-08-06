#include "savewithemptytitletest.h"
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
#include "../../xpiks-qt/Models/imageartwork.h"

QString SaveWithEmptyTitleTest::testName() {
    return QLatin1String("SaveWithEmptyTitleTest");
}

void SaveWithEmptyTitleTest::setup() {
}

int SaveWithEmptyTitleTest::doTest() {

    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/pixmap/seagull.jpg");

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
    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

    VERIFY(image->getImageSize().width() == 1920, "Image width was read incorrectly");
    VERIFY(image->getImageSize().height() == 1272, "Image height was read incorrectly");

    QString description = "Description for title";
    metadata->setDescription(description);
    metadata->setTitle(""); // title should be taken from keywords
    metadata->setIsSelected(true);

    bool doOverwrite = true, dontSaveBackups = false;

    QObject::connect(ioCoordinator, SIGNAL(metadataWritingFinished()), &waiter, SIGNAL(finished()));
    artItemsModel->saveSelectedArtworks(QVector<int>() << 0, doOverwrite, dontSaveBackups);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for writing metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while writing");

    artItemsModel->removeSelectedArtworks(QVector<int>() << 0);

    addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == 1, "Failed to add file");

    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    metadata = artItemsModel->getArtwork(0);
    const QString &title = metadata->getTitle();

    VERIFY(description == title, "Title was not set from description");

    return 0;
}
