#include "addfilesbasictest.h"
#include "integrationtestbase.h"
#include "../../xpiks-qt/Commands/commandmanager.h"

QString AddFilesBasicTest::testName() {
    return QLatin1String("AddFilesBasicTest");
}

void AddFilesBasicTest::setup() {
}

int AddFilesBasicTest::doTest() {
    return 0;
}

void AddFilesBasicTest::teardown() {
    m_CommandManager->cleanup();
}
