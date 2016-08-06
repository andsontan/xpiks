#include "undoaddwithvectorstest.h"
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

QString UndoAddWithVectorsTest::testName() {
    return QLatin1String("UndoAddWithVectorsTest");
}

void UndoAddWithVectorsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(true);
}

int UndoAddWithVectorsTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/mixed/026.jpg");
    files << getImagePathForTest("images-for-tests/mixed/0267.jpg");
    files << getImagePathForTest("images-for-tests/mixed/027.jpg");

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

    Models::ImageArtwork *firstImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(0));
    Q_ASSERT(firstImage != NULL);

    Models::ImageArtwork *secondImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(1));
    Q_ASSERT(secondImage != NULL);

    Models::ImageArtwork *thirdImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(2));
    Q_ASSERT(thirdImage != NULL);

    VERIFY(firstImage->hasVectorAttached(), "Vector wasn't attached to an image with vector");
    QString firstVector = firstImage->getAttachedVectorPath();

    VERIFY(!secondImage->hasVectorAttached(), "Vector was attached to an image without vector");

    VERIFY(thirdImage->hasVectorAttached(), "Vector wasn't attached to an image with vector");
    QString thirdVector = thirdImage->getAttachedVectorPath();

    artItemsModel->removeArtworksDirectory(0);
    VERIFY(artItemsModel->getArtworksCount() == 0, "Items were not removed");

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();

    bool undoSuccess = undoRedoManager->undoLastAction();
    VERIFY(undoSuccess, "Failed to Undo last action");

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(artItemsModel->getArtworksCount() == files.length(), "Items were not put back");

    firstImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(0));
    Q_ASSERT(firstImage != NULL);

    secondImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(1));
    Q_ASSERT(secondImage != NULL);

    thirdImage = dynamic_cast<Models::ImageArtwork*>(artItemsModel->getArtwork(2));
    Q_ASSERT(thirdImage != NULL);

    VERIFY(firstImage->hasVectorAttached(), "Vector wasn't attached to an image with vector");
    VERIFY(firstImage->getAttachedVectorPath() == firstVector, "Vector paths do not match for first item!")
    VERIFY(!secondImage->hasVectorAttached(), "Vector was attached to an image without vector");
    VERIFY(thirdImage->hasVectorAttached(), "Vector wasn't attached to an image with vector");
    VERIFY(thirdImage->getAttachedVectorPath() == thirdVector, "Vector paths do not match for third item");
    VERIFY(thirdVector != firstVector, "Vector got messed up");

    return 0;
}


