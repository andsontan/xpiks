#include "jsonmerge_tests.h"
#include "../Common/defines.h"
#include "../../xpiks-qt/Helpers/jsonhelper.h"
#include "../../xpiks-qt/Helpers/localconfig.h"

void JsonMergeTests::initTestCase() {
    LOG_DEBUG << "App path: " << QDir::currentPath();
}

void JsonMergeTests::mergeTwoFilesObjects() {
    QFileInfo infoOld("jsons-for-tests/old.json");
    QFileInfo infoNew("jsons-for-tests/new.json");
    QFileInfo infoGold("jsons-for-tests/gold.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig localConfigWith(pathWith);
    Helpers::LocalConfig localConfigTo(pathTo);
    Helpers::LocalConfig localConfigGold(pathGold);

    localConfigWith.initConfig();
    localConfigTo.initConfig();
    localConfigGold.initConfig();

    Helpers::mergeJson(localConfigTo.getConfig(), localConfigWith.getConfig(), 0, *this);

    QVERIFY(localConfigGold.getConfig() == localConfigTo.getConfig());
}

void JsonMergeTests::mergeTwoFilesStrings() {
    QFileInfo infoOld("jsons-for-tests/oldS.json");
    QFileInfo infoNew("jsons-for-tests/newS.json");
    QFileInfo infoGold("jsons-for-tests/goldS.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig localConfigWith(pathWith);
    Helpers::LocalConfig localConfigTo(pathTo);
    Helpers::LocalConfig localConfigGold(pathGold);

    localConfigWith.initConfig();
    localConfigTo.initConfig();
    localConfigGold.initConfig();

    Helpers::mergeJson(localConfigTo.getConfig(), localConfigWith.getConfig(), 0, *this);

    QVERIFY(localConfigGold.getConfig() == localConfigTo.getConfig());
}
