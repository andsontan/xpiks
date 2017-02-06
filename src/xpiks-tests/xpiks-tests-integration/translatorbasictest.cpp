#include "translatorbasictest.h"
#include <QDebug>
#include "../../xpiks-qt/Translation/translationmanager.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "signalwaiter.h"

QString TranslatorBasicTest::testName() {
    return QLatin1String("TranslatorBasicTest");
}

void TranslatorBasicTest::setup() {
    // BUMP
}

int TranslatorBasicTest::doTest() {
    auto *translationManager = m_CommandManager->getTranslationManager();
    QUrl pathToDict = getFilePathForTest("dicts-for-tests/eng_fin.ifo");

    bool success = translationManager->addDictionary(pathToDict);
    VERIFY(success, "Failed to add dictionary");

    translationManager->setSelectedDictionaryIndex(0);

    SignalWaiter waiter;
    QObject::connect(translationManager, SIGNAL(shortTranslationChanged()), &waiter, SIGNAL(finished()));

    translationManager->setQuery("test");
    if (!waiter.wait()) {
        VERIFY(false, "Timeout for waiting for translation");
    }

    VERIFY(!translationManager->getFullTranslation().simplified().isEmpty(), "Full translation is empty");
    VERIFY(!translationManager->getShortTranslation().simplified().isEmpty(), "Short translation is empty");
    qInfo() << "Translation arrived" << translationManager->getFullTranslation();

    return 0;
}


