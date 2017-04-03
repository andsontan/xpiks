#include "presetstest.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodelconfig.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include <QDir>

QString PresetsTest::testName() {
    return QLatin1String("PresetsTest");
}

void PresetsTest::setup() {
}

int PresetsTest::doTest() {
    auto *presetKeywordsModel = m_CommandManager->getPresetsModel();
    auto *presetKeywordsModelConfig = m_CommandManager->getPresetsModelConfig();

    typedef KeywordsPresets::PresetData PresetData;
    QVector<PresetData> presetDataVector;
    presetDataVector.push_back({QStringList() << QString("key1") << QString("key2"), QString("name1")});
    presetDataVector.push_back({QStringList() << QString("key3") << QString("key4"), QString("name2")});
    presetKeywordsModelConfig->initialize(presetDataVector);

    presetKeywordsModel->removeKeywordAt(0, 0);
    presetKeywordsModel->appendKeyword(1, "key5");
    presetKeywordsModel->addItem();
    presetKeywordsModel->setName(2, QString("name3"));
    presetKeywordsModel->appendKeyword(2, "key6");
    presetKeywordsModel->saveToConfig();

    QVector<PresetData> goldPresetDataVector;
    goldPresetDataVector.push_back({QStringList() << QString("key2"), QString("name1")});
    goldPresetDataVector.push_back({QStringList() << QString("key3") << QString("key4") << QString("key5"), QString("name2")});
    goldPresetDataVector.push_back({QStringList() << QString("key6"), QString("name3")});

    QThread::sleep(1);

    auto &presetDataNew = presetKeywordsModelConfig->getPresetData();
    int size = presetDataNew.size();
    VERIFY(size == goldPresetDataVector.size(), "Error in verifying config data size");

    for (int i = 0; i < size; i++) {
        VERIFY(presetDataNew[i].m_Keywords == goldPresetDataVector[i].m_Keywords, "Error in verifying config data item keywords");
        VERIFY(presetDataNew[i].m_Name == goldPresetDataVector[i].m_Name, "Error in verifying config data item name");
    }
    return 0;
}
