#include "autocompletebasictest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/AutoComplete/autocompleteservice.h"
#include "../../xpiks-qt/AutoComplete/autocompletemodel.h"

QString AutoCompleteBasicTest::testName() {
    return QLatin1String("AutoCompleteBasicTest");
}

void AutoCompleteBasicTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseAutoComplete(true);
}

int AutoCompleteBasicTest::doTest() {
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

    AutoComplete::AutoCompleteService *acService = m_CommandManager->getAutoCompleteService();
    AutoComplete::AutoCompleteModel *acModel = acService->getAutoCompleteModel();

    SignalWaiter completionWaiter;
    QObject::connect(acModel, SIGNAL(completionsUpdated()), &completionWaiter, SIGNAL(finished()));


    VERIFY(acModel->getCount() == 0, "AC model was not empty");

    m_CommandManager->autoCompleteKeyword("tes", metadata->getBasicModel());

    if (!completionWaiter.wait(10)) {
        VERIFY(false, "Timeout while waiting for the completion");
    }

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << acModel->getLastGeneratedCompletions();

    VERIFY(acModel->getCount() > 0, "AC model didn't receive the completions");
    VERIFY(acModel->containsWord("test"), "AC model has irrelevant results");

    m_CommandManager->autoCompleteKeyword("Tes", metadata->getBasicModel());

    if (!completionWaiter.wait(10)) {
        VERIFY(false, "Timeout while waiting for the completion");
    }

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << acModel->getLastGeneratedCompletions();

    VERIFY(acModel->getCount() > 0, "AC model didn't receive the completions second time");
    VERIFY(acModel->containsWord("test"), "AC model has irrelevant results");

    return 0;
}
