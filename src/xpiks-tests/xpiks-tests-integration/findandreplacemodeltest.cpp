#include "findandreplacemodeltest.h"
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
#include "../../xpiks-qt/Models/findandreplacemodel.h"

QString FindAndReplaceModelTest::testName() {
    return QLatin1String("FindAndReplaceModelTest");
}

void FindAndReplaceModelTest::setup() {

}

int FindAndReplaceModelTest::doTest() {
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

    artItemsModel->getArtwork(0)->setDescription("wall inside the Wall is not a wall");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    filteredModel->selectFilteredArtworks();

    Models::FindAndReplaceModel *findAndReplaceModel = m_CommandManager->getFindAndReplaceModel();

    findAndReplaceModel->setReplaceFrom("wall");
    findAndReplaceModel->setReplaceTo("wallpaper");
    findAndReplaceModel->setSearchWholeWords(true);
    findAndReplaceModel->initArtworksList();

    VERIFY(findAndReplaceModel->getArtworksCount() == 2, "Items are missing!");
    findAndReplaceModel->setItemSelected(1, false);

    int keywordsCount = artItemsModel->getBasicModel(0)->getKeywordsCount();

    findAndReplaceModel->replace();

    VERIFY(artItemsModel->getBasicModel(0)->getKeywordsCount() == (keywordsCount - 1), "Keyword duplicate wasn't removed");
    VERIFY(artItemsModel->getBasicModel(0)->getKeywords().last() == "wallpaper", "Keyword wasn't replaced");
    VERIFY(artItemsModel->getBasicModel(0)->getDescription() == "wallpaper inside the Wall is not a wallpaper", "Description wasn't replaced");

    return 0;
}



