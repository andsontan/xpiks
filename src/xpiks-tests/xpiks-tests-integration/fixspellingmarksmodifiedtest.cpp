#include "fixspellingmarksmodifiedtest.h"
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
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"
#include "../../xpiks-qt/Warnings/warningsservice.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "testshelpers.h"


QString FixSpellingMarksModifiedTest::testName() {
    return QLatin1String("FixSpellingMarksModifiedTest");
}

void FixSpellingMarksModifiedTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int FixSpellingMarksModifiedTest::doTest() {
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

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    Common::BasicMetadataModel *basicModel = qobject_cast<Common::BasicMetadataModel*>(filteredModel->getBasicModel(0));

    QString wrongWord = "abbreviatioe";
    basicModel->appendKeyword(wrongWord);

    QObject::connect(basicModel, SIGNAL(spellCheckErrorsChanged()),
                     &waiter, SIGNAL(finished()));

    m_CommandManager->submitItemForSpellCheck(basicModel);

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for initial spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(basicModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    artItemsModel->suggestCorrections(0);

    SpellCheck::SpellCheckSuggestionModel *spellSuggestor = m_CommandManager->getSpellSuggestionsModel();
    int rowCount = spellSuggestor->rowCount();
    VERIFY(rowCount > 0, "Spell suggestions are not set");

    for (int i = 0; i < rowCount; ++i) {
        SpellCheck::SpellSuggestionsItem *suggestionsItem = spellSuggestor->getItem(i);
        VERIFY(suggestionsItem->rowCount() > 0, "No spelling suggestion suggested");
        suggestionsItem->setReplacementIndex(0);
    }

    spellSuggestor->submitCorrections();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for corrected spellcheck results");
    }

    VERIFY(!basicModel->hasKeywordsSpellError(), "Keywords spell error was not fixed");
    VERIFY(artItemsModel->getArtwork(0)->isModified(), "Artwork was not modified");

    return 0;
}
