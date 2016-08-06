#include "spellcheckundotest.h"
#include <QUrl>
#include <QFileInfo>
#include <QThread>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/SpellCheck/spellsuggestionsitem.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"

QString SpellCheckUndoTest::testName() {
    return QLatin1String("SpellCheckUndoTest");
}

void SpellCheckUndoTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int SpellCheckUndoTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/vector/026.jpg");

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

    QString wrongWord = "abbreviatioe";
    metadata->setDescription(metadata->getDescription() + ' ' + wrongWord);
    metadata->setTitle(metadata->getTitle() + ' ' + wrongWord);
    metadata->appendKeyword("correct part " + wrongWord);
    metadata->setIsSelected(true);

    // wait for after-add spellchecking
    QThread::sleep(1);

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));

    filteredModel->spellCheckSelected();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);
    Common::BasicKeywordsModel *basicKeywordsModel = metadata->getKeywordsModel();

    VERIFY(basicKeywordsModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(basicKeywordsModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    filteredModel->clearKeywords(0);
    QThread::sleep(1);

    VERIFY(!basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error not cleared");

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();
    undoRedoManager->undoLastAction();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for second spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(basicKeywordsModel->hasDescriptionSpellError(), "Description spell error not detected on the second time");
    VERIFY(basicKeywordsModel->hasTitleSpellError(), "Title spell error not detected on the second time");
    VERIFY(basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected on the second time");

    return 0;
}




