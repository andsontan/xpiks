
#include "jsonmerge_tests.h"
#include "../../xpiks-qt/Helpers/jsonhelper.h"
#include "../../xpiks-qt/Helpers/localconfig.h"
#include "../Common/defines.h"

void JsonMergeTests::mergeTwoFilesObjects() {

    LOG_WARNING << "App path : " << QDir::currentPath();
    QFileInfo infoOld("jsons-for-tests/old.json");
    QFileInfo infoNew("jsons-for-tests/new.json");
    QFileInfo infoGold("jsons-for-tests/gold.json");
    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();
    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig locConfWith(pathWith);
    Helpers::LocalConfig locConfTo(pathTo);
    Helpers::LocalConfig locConfGold(pathGold);

    locConfWith.initConfig();
    locConfTo.initConfig();
    locConfGold.initConfig();
    Helpers::mergeJson(locConfTo.getConfig(),locConfWith.getConfig(),0,*this);

    QVERIFY(locConfGold.getConfig() == locConfTo.getConfig());




}


void JsonMergeTests::mergeTwoFilesStrings() {

    LOG_WARNING << "App path : " << QDir::currentPath();
    QFileInfo infoOld("jsons-for-tests/oldS.json");
    QFileInfo infoNew("jsons-for-tests/newS.json");
    QFileInfo infoGold("jsons-for-tests/goldS.json");
    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();
    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig locConfWith(pathWith);
    Helpers::LocalConfig locConfTo(pathTo);
    Helpers::LocalConfig locConfGold(pathGold);

    locConfWith.initConfig();
    locConfTo.initConfig();
    locConfGold.initConfig();
    Helpers::mergeJson(locConfTo.getConfig(),locConfWith.getConfig(),0,*this);

    QVERIFY(locConfGold.getConfig() == locConfTo.getConfig());




}
