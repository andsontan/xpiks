#include "clearmetadatatest.h"
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
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"

QString ClearMetadataTest::testName() {
    return QLatin1String("ClearMetadataTest");
}

void ClearMetadataTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(false);
}

int ClearMetadataTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/pixmap/seagull-for-clear.jpg");

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

    QStringList expectedKeywords = QString("picture,seagull,bird").split(',');

    VERIFY(expectedKeywords == keywords, "Keywords are not the same!");
    VERIFY(metadata->getDescription() == "Seagull description", "Description is not the same!");
    VERIFY(metadata->getTitle() == "Seagull title", "Title is not the same!");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    metadata->setIsSelected(true);

    filteredModel->removeMetadataInSelected();

    bool doOverwrite = true, dontSaveBackups = false;
    QObject::connect(ioCoordinator, SIGNAL(metadataWritingFinished()), &waiter, SIGNAL(finished()));

    artItemsModel->saveSelectedArtworks(QVector<int>() << 0, doOverwrite, dontSaveBackups);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for writing metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while writing");

    filteredModel->removeSelectedArtworks();
    addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add file after removal");

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    metadata = artItemsModel->getArtwork(0);

    VERIFY(metadata->getBasicModel()->isDescriptionEmpty(), "Description was not empty");
    VERIFY(metadata->getBasicModel()->isTitleEmpty(), "Title was not empty");
    VERIFY(metadata->getBasicModel()->areKeywordsEmpty(), "Keywords were not empty");

    return 0;
}


