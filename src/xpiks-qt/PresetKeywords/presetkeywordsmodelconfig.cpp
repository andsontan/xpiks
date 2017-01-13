#include "presetkeywordsmodelconfig.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../Conectivity/apimanager.h"

namespace  KeywordsPreset {
#define OVERWRITE_KEY QLatin1String("overwrite")
#define PRESETKEYS_KEY QLatin1String("presetkeys")
#define LOCAL_PRESETKEYWORDS_LIST_FILE "preset_keywords.json"
#define OVERWRITE_PRESETS_CONFIG false

    PresetKeywordsModelConfig::PresetKeywordsModelConfig(QObject *parent):
        Models::AbstractConfigUpdaterModel(OVERWRITE_PRESETS_CONFIG, parent)
    {}

    void PresetKeywordsModelConfig::initializeConfigs() {
        LOG_DEBUG << "#";
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_PRESETKEYWORDS_LIST_FILE);
        } else {
            localConfigPath = LOCAL_PRESETKEYWORDS_LIST_FILE;
        }

        auto &apiManager = Conectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getPresetsSourceAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);
        emit presetsUpdated();
    }

    void PresetKeywordsModelConfig::saveFromModel(const std::vector<Preset> &presets) {
        int size = presets.size();
        LOG_INTEGR_TESTS_OR_DEBUG << size;

        m_PresetData.resize(size);
        for (int i = 0; i < size; i++) {
            auto &item = presets[i];
            auto &name = item.m_PresetName;
            auto &keywordsModel = item.m_KeywordsModel;
            auto keywords = keywordsModel->getKeywords();
            m_PresetData[i].m_Keys = keywords;
            m_PresetData[i].m_Name = name;
        }
#ifndef INTEGRATION_TESTS
        writeToConfig();
#endif
    }

    bool PresetKeywordsModelConfig::parseConfig(const QJsonDocument &document) {
        LOG_INTEGR_TESTS_OR_DEBUG << document;
        bool anyError = false;

        do {
            if (!document.isObject()) {
                LOG_WARNING << "Json document is not an object";
                anyError = true;
                break;
            }

            QJsonObject rootObject = document.object();
            if (!rootObject.contains(PRESETKEYS_KEY)) {
                LOG_WARNING << "There's no presetKeys key in json";
                anyError = true;
                break;
            }

            QJsonValue presetArrayValue = rootObject[PRESETKEYS_KEY];
            if (!presetArrayValue.isArray()) {
                LOG_WARNING << "presetArray array object is not an array";
                anyError = true;
                break;
            }

            QJsonArray presetArray = presetArrayValue.toArray();
            parsePresetArray(presetArray);
        } while (false);

        return anyError;
    }

    void PresetKeywordsModelConfig::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        bool overwrite = false;

        if (!overwriteLocal && remoteDocument.isObject()) {
            QJsonObject rootObject = remoteDocument.object();
            if (rootObject.contains(OVERWRITE_KEY)) {
                QJsonValue overwriteValue = rootObject[OVERWRITE_KEY];
                if (overwriteValue.isBool()) {
                    overwrite = overwriteValue.toBool();
                    LOG_DEBUG << "Overwrite flag present in the config:" << overwrite;
                } else {
                    LOG_WARNING << "Overwrite flag is not boolean";
                }
            }
        } else {
            overwrite = overwriteLocal;
        }

        Models::AbstractConfigUpdaterModel::processRemoteConfig(remoteDocument, overwrite);
    }

    int PresetKeywordsModelConfig::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        // values are always considered equal. This may lead to loss of local changes.
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
    }

    void PresetKeywordsModelConfig::parsePresetArray(const QJsonArray &array) {
        QStringList keys;
        int size = array.size();

        keys.reserve(size);

        for (int i = 0; i < size; ++i) {
            QJsonValue item = array.at(i);

            if (!item.isObject()) {
                continue;
            }

            QJsonObject presetKeywordsItem = item.toObject();
            if (presetKeywordsItem.size() != 1) {
                continue;
            }

            QString presetKey = presetKeywordsItem.keys()[0];
            QJsonValue presetKeywordsValue = presetKeywordsItem.value(presetKey);

            if (!presetKeywordsValue.isArray()) {
                continue;
            }

            QJsonArray jsonArray = presetKeywordsValue.toArray();
            QStringList list;
            int size = jsonArray.size();
            list.reserve(size);
            for (auto item: jsonArray) {
                if (!item.isString()) {
                    LOG_WARNING << "value is not string";
                    continue;
                }

                list.append(item.toString());
            }

            m_PresetData.push_back({list, presetKey});
        }
    }

    void PresetKeywordsModelConfig::writeToConfig() {
        QJsonArray jsonArray;

        for (auto &item : m_PresetData) {
            QJsonObject object;
            QJsonArray keys;
            LOG_WARNING << item.m_Name<< " " << item.m_Keys;
            keys = QJsonArray::fromStringList(item.m_Keys);
            object.insert(item.m_Name, keys);
            jsonArray.append(object);
        }

        QJsonObject topObject;
        topObject.insert(OVERWRITE_KEY, OVERWRITE_PRESETS_CONFIG);
        topObject.insert(PRESETKEYS_KEY, jsonArray);
        QJsonDocument doc;
        doc.setObject(topObject);
        LOG_WARNING << doc;
        Helpers::LocalConfig &localConfig = getLocalConfig();
        localConfig.setConfig(doc);
        localConfig.saveToFile();
    }

    void PresetKeywordsModelConfig::initialize(const QVector<PresetData> &presetData){
        m_PresetData = presetData;
        emit presetsUpdated();
    }

    const QVector<PresetData> & PresetKeywordsModelConfig::getPresetData(){
        return m_PresetData;
    }
}
