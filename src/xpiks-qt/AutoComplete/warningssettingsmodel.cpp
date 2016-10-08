/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
#include "warningssettingsmodel.h"
#include "../Conectivity/apimanager.h"
#include <QStandardPaths>
#include <QDir>

#define OVERWRITE_WARNINGS_CONFIG false
#define LOCAL_WARNINGS_SETTINGS_FILE QLatin1String("warnings_settings.json")

#define OVERWRITE_KEY QLatin1String("overwrite")
#define SETTINGS_KEY QLatin1String("settings")
#define ALLOWED_CHARACTERS QLatin1String("chars")
#define MIN_MPIXELS QLatin1String("min_mpixels")
#define MAX_KEYWORDS_COUNT QLatin1String("max_keywords_count")
#define MAX_DESCRIPTION_LENGTH QLatin1String("max_description_length")

namespace  AutoComplete {
    WarningsSettingsModel::WarningsSettingsModel():
        Models::AbstractConfigUpdaterModel(OVERWRITE_WARNINGS_CONFIG),
        m_AllowedFilenameCharacters(".,_-@ "),
        m_MinMegapixels(4),
        m_MaxKeywordsCount(50),
        m_MaxDescriptionLength(200)
    {}

    void WarningsSettingsModel::initializeConfigs() {
        LOG_DEBUG <<'#';
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;

        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_WARNINGS_SETTINGS_FILE);
        } else {
            localConfigPath = LOCAL_WARNINGS_SETTINGS_FILE;
        }

        auto &apiManager = Conectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getWarningSettingsAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);

        const Helpers::LocalConfig &localConfig = getLocalConfig();
        const QJsonDocument &localDocument = localConfig.getConfig();
        parseConfig(localDocument);
    }

    void WarningsSettingsModel::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
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

    bool WarningsSettingsModel::parseConfig(const QJsonDocument &document) {
        LOG_DEBUG<<document;
        bool anyError = false;

        do {
            if (!document.isObject()) {
                LOG_WARNING << "Json document is not an object";
                anyError = true;
                break;
            }

            QJsonObject rootObject = document.object();
            if (!rootObject.contains(SETTINGS_KEY)) {
                LOG_WARNING << "There's no settings key in json";
                anyError = true;
                break;
            }

            QJsonValue settingsValue = rootObject[SETTINGS_KEY];
            if (!settingsValue.isObject()) {
                LOG_WARNING << "Settings object is not an object";
                anyError = true;
                break;
            }

            QJsonObject settingsObject = settingsValue.toObject();

            QJsonValue allowedCharacters = settingsObject[ALLOWED_CHARACTERS];
            if (!allowedCharacters.isString()) {
                LOG_WARNING << "ALLOWED_CHARACTERS value is not string";
                anyError = true;
                break;
            }

            m_AllowedFilenameCharacters = allowedCharacters.toString();

            QJsonValue minMPixels = settingsObject[MIN_MPIXELS];
            if (!minMPixels.isDouble()) {
                LOG_WARNING << "MIN_MPIXELS value is not number";
                anyError = true;
                break;
            }

            m_MinMegapixels = minMPixels.toDouble();
            QJsonValue maxKeywordsCount = settingsObject[MAX_KEYWORDS_COUNT];
            if (!maxKeywordsCount.isDouble()) {
                LOG_WARNING << "MAX_KEYWORDS_COUNT value is not number";
                anyError = true;
                break;
            }

            m_MaxKeywordsCount = maxKeywordsCount.toInt();
            QJsonValue maxDescriptionCount = settingsObject[MAX_DESCRIPTION_LENGTH];
            if (!maxDescriptionCount.isDouble()) {
                LOG_WARNING << "MAX_DESCRIPTION_LENGTH value is not number";
                anyError = true;
                break;
            }

            m_MaxDescriptionLength = maxDescriptionCount.toInt();
        } while (false);

        LOG_DEBUG<<anyError;
        return anyError;
    }

    int WarningsSettingsModel::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
    }
}
