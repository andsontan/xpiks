#include "combinededitfixspellingtest.h"
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

QString CombinedEditFixSpellingTest::testName() {
    return QLatin1String("CombinedEditFixSpellingTest");
}

void CombinedEditFixSpellingTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int CombinedEditFixSpellingTest::doTest() {
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

    // wait for after-add spellchecking
    QThread::sleep(1);

    QString wrongWord = "abbreviatioe";

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    Common::BasicKeywordsModel *basicModel = qobject_cast<Common::BasicKeywordsModel*>(filteredModel->getKeywordsModel(0));

    QString nextDescription = basicModel->getDescription() + ' ' + wrongWord;
    basicModel->setDescription(nextDescription);

    QObject::connect(basicModel, SIGNAL(spellCheckErrorsChanged()),
                     &waiter, SIGNAL(finished()));

    m_CommandManager->submitItemForSpellCheck(basicModel);

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for initial spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(basicModel->hasDescriptionSpellError(), "Description spell error not detected");

    filteredModel->selectFilteredArtworks();
    filteredModel->combineSelectedArtworks();

    // wait for finding suggestions
    QThread::sleep(1);

    Models::CombinedArtworksModel *combinedModel = m_CommandManager->getCombinedArtworksModel();
    combinedModel->suggestCorrections();

    SpellCheck::SpellCheckSuggestionModel *spellSuggestor = m_CommandManager->getSpellSuggestionsModel();
    int rowCount = spellSuggestor->rowCount();
    VERIFY(rowCount > 0, "Spell suggestions are not set");

    for (int i = 0; i < rowCount; ++i) {
        SpellCheck::SpellSuggestionsItem *suggestionsItem = spellSuggestor->getItem(i);
        VERIFY(suggestionsItem->rowCount() > 0, "No spelling suggestion suggested");
        suggestionsItem->setReplacementIndex(0);
    }

    Common::BasicKeywordsModel *combinedKeywordsModel = combinedModel->getBasicKeywordsModel();
    VERIFY(combinedKeywordsModel->hasDescriptionSpellError(), "Description spell error was not propagated");

    SignalWaiter combinedEditWaiter;
    QObject::connect(combinedKeywordsModel, SIGNAL(spellCheckErrorsChanged()),
                     &combinedEditWaiter, SIGNAL(finished()));

    spellSuggestor->submitCorrections();

    if (!combinedEditWaiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for corrected spellcheck results after replace");
    }

    VERIFY(!combinedKeywordsModel->hasDescriptionSpellError(), "Description spell error was not fixed");

    QString correctDescription = combinedKeywordsModel->getDescription();
    combinedModel->saveEdits();

    VERIFY(basicModel->getDescription() == correctDescription, "Description was not saved");

    return 0;
}
