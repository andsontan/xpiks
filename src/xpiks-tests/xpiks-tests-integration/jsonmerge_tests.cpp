#include "jsonmerge_tests.h"
#include <QDir>
#include "../../xpiks-qt/Common/defines.h"
#include "../../xpiks-qt/Helpers/jsonhelper.h"
#include "../../xpiks-qt/Helpers/localconfig.h"

QString JsonMergeTests::testName() {
    return QLatin1String("jsonmergetests");
}

void JsonMergeTests::setup() {
    LOG_DEBUG << "App path: " << QDir::currentPath();
}

int JsonMergeTests::doTest() {
    mergeTwoFilesObjects();
    mergeTwoFilesStrings();
    return 0;
}

int JsonMergeTests::mergeTwoFilesObjects() {
    QFileInfo infoOld("jsons-for-tests/old.json");
    QFileInfo infoNew("jsons-for-tests/new.json");
    QFileInfo infoGold("jsons-for-tests/gold.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    VERIFY(infoOld.exists(),"infoOld does not exist");
    VERIFY(infoNew.exists(),"infoNew does not exist");
    VERIFY(infoGold.exists(),"infoGold does not exist");

    Helpers::LocalConfig localConfigWith;
    Helpers::LocalConfig localConfigTo;
    Helpers::LocalConfig localConfigGold;

    localConfigWith.initConfig(pathWith);
    localConfigTo.initConfig(pathTo);
    localConfigGold.initConfig(pathGold);

    Helpers::mergeJson(localConfigWith.getConfig(), localConfigTo.getConfig(), 0, *this);

    VERIFY(localConfigGold.getConfig() == localConfigTo.getConfig(),"generated json does not equal localConfigGold");
    return 0;
}

int JsonMergeTests::mergeTwoFilesStrings() {
    QFileInfo infoOld("jsons-for-tests/oldS.json");
    QFileInfo infoNew("jsons-for-tests/newS.json");
    QFileInfo infoGold("jsons-for-tests/goldS.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    VERIFY(infoOld.exists(),"infoOld does not exist");
    VERIFY(infoNew.exists(),"infoNew does not exist");
    VERIFY(infoGold.exists(),"infoGold does not exist");

    Helpers::LocalConfig localConfigWith;
    Helpers::LocalConfig localConfigTo;
    Helpers::LocalConfig localConfigGold;

    localConfigWith.initConfig(pathWith);
    localConfigTo.initConfig(pathTo);
    localConfigGold.initConfig(pathGold);

    Helpers::mergeJson(localConfigWith.getConfig(), localConfigTo.getConfig(), 0, *this);

    const QJsonDocument &goldConfig = localConfigGold.getConfig();
    const QJsonDocument &testConfig = localConfigTo.getConfig();
    QJsonObject goldObject = goldConfig.object();
    QJsonObject testObject = testConfig.object();
    QJsonArray goldArray = goldObject["ftp_list"].toArray();
    QJsonArray testArray = testObject["ftp_list"].toArray();
    QSet<QString> goldSet;
    QSet<QString> testSet;
    int goldSize = goldArray.size();
    int testSize = testArray.size();

    for (int i = 0; i < goldSize; i++) {
        goldSet.insert(goldArray[i].toString());
    }

    for (int i = 0; i < testSize; i++) {
        testSet.insert(testArray[i].toString());
    }

    VERIFY(testSet == goldSet,"goldSet does not equal testSet");
    return 0;
}
