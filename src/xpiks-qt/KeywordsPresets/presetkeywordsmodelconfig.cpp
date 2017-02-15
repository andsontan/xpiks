/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "presetkeywordsmodelconfig.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../Conectivity/apimanager.h"

namespace KeywordsPresets {
#define OVERWRITE_KEY QLatin1String("overwrite")
#define PRESETKEYS_KEY QLatin1String("presetkeywords")
#define LOCAL_PRESETKEYWORDS_LIST_FILE "keywords_presets.json"
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

    void PresetKeywordsModelConfig::saveFromModel(const std::vector<PresetModel *> &presets) {
        int size = (int)presets.size();
        LOG_INTEGR_TESTS_OR_DEBUG << size;

        m_PresetData.resize(size);
        for (int i = 0; i < size; i++) {
            auto *item = presets[i];
            auto &name = item->m_PresetName;
            auto &keywordsModel = item->m_KeywordsModel;

            m_PresetData[i].m_Keywords = keywordsModel.getKeywords();
            m_PresetData[i].m_Name = name;
        }

#if !defined(INTEGRATION_TESTS) && !defined(CORE_TESTS)
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

        for (auto &item: m_PresetData) {
            QJsonObject object;
            QJsonArray keywords;
            keywords = QJsonArray::fromStringList(item.m_Keywords);
            object.insert(item.m_Name, keywords);
            jsonArray.append(object);
        }

        QJsonObject rootObject;
        rootObject.insert(OVERWRITE_KEY, OVERWRITE_PRESETS_CONFIG);
        rootObject.insert(PRESETKEYS_KEY, jsonArray);
        QJsonDocument doc;
        doc.setObject(rootObject);

        Helpers::LocalConfig &localConfig = getLocalConfig();
        localConfig.setConfig(doc);
        localConfig.saveToFile();
    }

    void PresetKeywordsModelConfig::initialize(const QVector<PresetData> &presetData){
        m_PresetData = presetData;
        emit presetsUpdated();
    }
}
