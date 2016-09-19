#include "spellingproduceswarningstest.h"
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

QString SpellingProducesWarningsTest::testName() {
    return QLatin1String("SpellingProducesWarningsTest");
}

void SpellingProducesWarningsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int SpellingProducesWarningsTest::doTest() {
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

    sleepWait(3, [metadata]() {
        return !Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle) &&
                !Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription) &&
                !Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords);
    });

    VERIFY(!Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle), "Error for reading title");
    VERIFY(!Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription), "Error for reading description");
    VERIFY(!Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords), "Error for reading keywords");

    QString wrongWord = "abbreviatioe";
    metadata->setDescription(metadata->getDescription() + ' ' + wrongWord);
    metadata->setTitle(metadata->getTitle() + ' ' + wrongWord);
    metadata->appendKeyword("correct part " + wrongWord);
    metadata->setIsSelected(true);

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();

    SignalWaiter spellingWaiter;
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &spellingWaiter, SIGNAL(finished()));

    // wait for checking warnings
    Warnings::WarningsService *warningsService = m_CommandManager->getWarningsService();
    SignalWaiter warningsQueueWaiter;
    QObject::connect(warningsService, SIGNAL(queueIsEmpty()), &warningsQueueWaiter, SIGNAL(finished()));

    filteredModel->spellCheckSelected();

    if (!spellingWaiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    LOG_INFO << "Spellchecking finished. Waiting for warnings...";

    if (!warningsQueueWaiter.wait(5)) {
        if (metadata->getWarningsFlags() == Common::WarningFlags::None) {
            VERIFY(false, "Timeout for waiting for warnings checker");
        }
    }

    Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();

    VERIFY(keywordsModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(keywordsModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(keywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    VERIFY(Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle),
           "Warning was not produced for title spelling error");
    VERIFY(Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription),
           "Warning was not produced for description spelling error");
    VERIFY(Common::HasFlag(metadata->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords),
           "Warning was not produced for keywords spelling error");

    return 0;
}
