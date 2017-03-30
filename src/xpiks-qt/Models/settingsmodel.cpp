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

#include "settingsmodel.h"
#include <QQmlEngine>
#include "../Common/defines.h"

#ifdef Q_OS_MAC
#  define DEFAULT_EXIFTOOL "/usr/bin/exiftool"
#elif APPVEYOR
#  define DEFAULT_EXIFTOOL "c:/projects/xpiks-deps/windows-3rd-party-bin/exiftool.exe"
#else
#  define DEFAULT_EXIFTOOL "exiftool"
#endif
#ifdef QT_DEBUG
#define SETTINGS_FILE "debug_settings.json"
#elif INTEGRATION_TESTS
#define SETTINGS_FILE "integration_settings.json"
#else
#define SETTINGS_FILE "settings.json"
#endif

#define DEFAULT_DICT_PATH ""
#define DEFAULT_USE_MASTERPASSWORD false
#define DEFAULT_UPLOAD_TIMEOUT 10
#define DEFAULT_USE_CONFIRMATIONS false
#define DEFAULT_SAVE_BACKUPS true
#define DEFAULT_KEYWORD_SIZE_SCALE 1.0
#define DEFAULT_DISMISS_DURATION 10
#define DEFAULT_MAX_PARALLEL_UPLOADS 2
#define DEFAULT_FIT_SMALL_PREVIEW false
#define DEFAULT_SEARCH_USING_AND true
#define DEFAULT_SEARCH_BY_FILEPATH true
#define DEFAULT_SCROLL_SPEED_SCALE 1.0
#define DEFAULT_USE_SPELL_CHECK true
#define DEFAULT_HAVE_USER_CONSENT false
#define DEFAULT_COLLECT_USER_STATISTICS true
#define DEFAULT_CHECK_FOR_UPDATES true
#define DEFAULT_AUTO_DOWNLOAD_UPDATES true
#define DEFAULT_APP_WIDTH 900
#define DEFAULT_APP_HEIGHT 725
#define DEFAULT_APP_POSITION -1
#define DEFAULT_AUTO_FIND_VECTORS true
#define DEFAULT_LOCALE "en_US"
#define DEFAULT_SELECTED_THEME_INDEX 0
#define DEFAULT_USE_AUTO_COMPLETE true
#define DEFAULT_USE_EXIFTOOL false
#define DEFAULT_USE_PROXY false
#define DEFAULT_PROXY_HOST ""
#define DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH 300
#define DEFAULT_SELECTED_DICT_INDEX -1

#ifndef INTEGRATION_TESTS
#define DEFAULT_AUTO_CACHE_IMAGES true
#define DEFAULT_VERBOSE_UPLOAD true
#else
#define DEFAULT_AUTO_CACHE_IMAGES false
#define DEFAULT_VERBOSE_UPLOAD false
#endif

namespace Models {
    SettingsModel::SettingsModel(QObject *parent) :
        QObject(parent),
        m_ExifToolPath(DEFAULT_EXIFTOOL),
        m_DictPath(DEFAULT_DICT_PATH),
        m_SelectedLocale(DEFAULT_LOCALE),
        m_KeywordSizeScale(DEFAULT_KEYWORD_SIZE_SCALE),
        m_ScrollSpeedScale(DEFAULT_SCROLL_SPEED_SCALE),
        m_ArtworkEditRightPaneWidth(DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH),
        m_UploadTimeout(DEFAULT_UPLOAD_TIMEOUT),
        m_DismissDuration(DEFAULT_DISMISS_DURATION),
        m_MaxParallelUploads(DEFAULT_MAX_PARALLEL_UPLOADS),
        m_SelectedThemeIndex(DEFAULT_SELECTED_THEME_INDEX),
        m_SelectedDictIndex(DEFAULT_SELECTED_DICT_INDEX),
        m_MustUseMasterPassword(DEFAULT_USE_MASTERPASSWORD),
        m_MustUseConfirmations(DEFAULT_USE_CONFIRMATIONS),
        m_SaveBackups(DEFAULT_SAVE_BACKUPS),
        m_FitSmallPreview(DEFAULT_FIT_SMALL_PREVIEW),
        m_SearchUsingAnd(DEFAULT_SEARCH_USING_AND),
        m_SearchByFilepath(DEFAULT_SEARCH_BY_FILEPATH),
        m_UseSpellCheck(DEFAULT_USE_SPELL_CHECK),
        m_UserStatistics(DEFAULT_COLLECT_USER_STATISTICS),
        m_CheckForUpdates(DEFAULT_CHECK_FOR_UPDATES),
        m_AutoDownloadUpdates(DEFAULT_AUTO_DOWNLOAD_UPDATES),
        m_DictsPathChanged(false),
        m_AutoFindVectors(DEFAULT_AUTO_FIND_VECTORS),
        m_UseAutoComplete(DEFAULT_USE_AUTO_COMPLETE),
        m_UseExifTool(DEFAULT_USE_EXIFTOOL),
        m_UseProxy(DEFAULT_USE_PROXY),
        m_AutoCacheImages(DEFAULT_AUTO_CACHE_IMAGES),
        m_VerboseUpload(DEFAULT_VERBOSE_UPLOAD)
    {
    }

    void SettingsModel::initializeConfigs() {
        LOG_DEBUG << "#";

        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(SETTINGS_FILE);
        } else {
            localConfigPath = SETTINGS_FILE;
        }

        m_Config.initConfig(localConfigPath);

        QJsonDocument &doc = m_Config.getConfig();
        if (doc.isObject()) {
            m_SettingsJson = doc.object();
        } else {
            LOG_WARNING << "JSON document doesn't contain an object";
        }
    }

    void SettingsModel::sync() {
        LOG_DEBUG << "Syncing settings";

        QJsonDocument doc;
        doc.setObject(m_SettingsJson);

        m_Config.setConfig(doc);
        m_Config.saveToFile();
    }

    void SettingsModel::saveExiftool() {
        LOG_DEBUG << "#";
        setValue(Constants::pathToExifTool, m_ExifToolPath);
        sync();
    }

    void SettingsModel::saveLocale() {
        LOG_DEBUG << "#";
        setValue(Constants::selectedLocale, m_SelectedLocale);
        sync();
    }

    ProxySettings *SettingsModel::retrieveProxySettings() {
        ProxySettings *result = nullptr;

        if (getUseProxy()) {
            result = &m_ProxySettings;
        }

        return result;
    }

    void SettingsModel::deserializeProxyFromSettings(const QString &serialized) {
        QByteArray originalData;
        originalData.append(serialized.toLatin1());
        QByteArray serializedBA = QByteArray::fromBase64(originalData);

        QDataStream ds(&serializedBA, QIODevice::ReadOnly);
        ds >> m_ProxySettings;
    }

    void SettingsModel::resetAllValues() {
        LOG_DEBUG << "#";
        resetToDefault();
        saveAllValues();
        emit settingsReset();
    }

    void SettingsModel::saveAllValues() {
        LOG_INFO << "#";

        using namespace Constants;

        setValue(pathToExifTool, m_ExifToolPath);
        setValue(dictPath, m_DictPath);
        setValue(oneUploadSecondsTimeout, m_UploadTimeout);
        setValue(useMasterPassword, m_MustUseMasterPassword);
        setValue(useConfirmationDialogs, m_MustUseConfirmations);
        setValue(saveBackups, m_SaveBackups);
        setValue(keywordSizeScale, m_KeywordSizeScale);
        setValue(dismissDuration, m_DismissDuration);
        setValue(maxParallelUploads, m_MaxParallelUploads);
        setValue(fitSmallPreview, m_FitSmallPreview);
        setValue(searchUsingAnd, m_SearchUsingAnd);
        setValue(searchByFilepath, m_SearchByFilepath);
        setValue(scrollSpeedSensivity, m_ScrollSpeedScale);
        setValue(useSpellCheck, m_UseSpellCheck);
        setValue(userStatistics, m_UserStatistics);
        setValue(checkForUpdates, m_CheckForUpdates);
        setValue(autoDownloadUpdates, m_AutoDownloadUpdates);
        setValue(autoFindVectors, m_AutoFindVectors);
        setValue(selectedThemeIndex, m_SelectedThemeIndex);
        setValue(useAutoComplete, m_UseAutoComplete);
        setValue(useExifTool, m_UseExifTool);
        setValue(useProxy, m_UseProxy);
        setValue(proxyHost, serializeProxyForSettings(m_ProxySettings));
        setValue(cacheImagesAutomatically, m_AutoCacheImages);
        setValue(artworkEditRightPaneWidth, m_ArtworkEditRightPaneWidth);
        setValue(verboseUpload, m_VerboseUpload);

        if (!m_MustUseMasterPassword) {
            setValue(masterPasswordHash, "");
        }

        emit keywordSizeScaleChanged(m_KeywordSizeScale);

#if defined(Q_OS_LINUX)
        if (m_DictsPathChanged) {
            // TODO: check if need to restart depending on path
            m_CommandManager->restartSpellChecking();
            m_DictsPathChanged = false;
        }
#endif

#ifdef Q_OS_MAC
        if (m_UseExifTool) {
            m_CommandManager->autoDiscoverExiftool();
        }
#endif

        sync();
        emit settingsUpdated();
    }

    void SettingsModel::clearMasterPasswordSettings() {
        setMustUseMasterPassword(false);
        setValue(Constants::masterPasswordHash, "");
        setValue(Constants::useMasterPassword, false);
        sync();
    }

    void SettingsModel::resetExifTool() {
        setExifToolPath(DEFAULT_EXIFTOOL);
    }

    void SettingsModel::resetDictPath() {
        setDictionaryPath(DEFAULT_DICT_PATH);
    }

    void SettingsModel::readAllValues() {
        LOG_INFO << "#";

        using namespace Constants;

        setExifToolPath(stringValue(pathToExifTool, DEFAULT_EXIFTOOL));
        setDictionaryPath(stringValue(dictPath, DEFAULT_DICT_PATH));
        setUploadTimeout(intValue(oneUploadSecondsTimeout, DEFAULT_UPLOAD_TIMEOUT));
        setMustUseMasterPassword(boolValue(useMasterPassword, DEFAULT_USE_MASTERPASSWORD));
        setMustUseConfirmations(boolValue(useConfirmationDialogs, DEFAULT_USE_CONFIRMATIONS));
        setSaveBackups(boolValue(saveBackups, DEFAULT_SAVE_BACKUPS));
        setKeywordSizeScale(doubleValue(keywordSizeScale, DEFAULT_KEYWORD_SIZE_SCALE));
        setDismissDuration(intValue(dismissDuration, DEFAULT_DISMISS_DURATION));
        setMaxParallelUploads(intValue(maxParallelUploads, DEFAULT_MAX_PARALLEL_UPLOADS));
        setFitSmallPreview(boolValue(fitSmallPreview, DEFAULT_FIT_SMALL_PREVIEW));
        setSearchUsingAnd(boolValue(searchUsingAnd, DEFAULT_SEARCH_USING_AND));
        setSearchByFilepath(boolValue(searchByFilepath, DEFAULT_SEARCH_BY_FILEPATH));
        setScrollSpeedScale(doubleValue(scrollSpeedSensivity, DEFAULT_SCROLL_SPEED_SCALE));
        setUseSpellCheck(boolValue(useSpellCheck, DEFAULT_USE_SPELL_CHECK));
        setUserStatistics(boolValue(userStatistics, DEFAULT_COLLECT_USER_STATISTICS));
        setCheckForUpdates(boolValue(checkForUpdates, DEFAULT_CHECK_FOR_UPDATES));
        setAutoDownloadUpdates(boolValue(autoDownloadUpdates, DEFAULT_AUTO_DOWNLOAD_UPDATES));
        setAutoFindVectors(boolValue(autoFindVectors, DEFAULT_AUTO_FIND_VECTORS));
        setSelectedLocale(stringValue(selectedLocale, DEFAULT_LOCALE));
        setSelectedThemeIndex(intValue(selectedThemeIndex, DEFAULT_SELECTED_THEME_INDEX));
        setUseAutoComplete(boolValue(useAutoComplete, DEFAULT_USE_AUTO_COMPLETE));
        setUseExifTool(boolValue(useExifTool, DEFAULT_USE_EXIFTOOL));
        setUseProxy(boolValue(useProxy, DEFAULT_USE_PROXY));
        setVerboseUpload(boolValue(verboseUpload, DEFAULT_VERBOSE_UPLOAD));

        deserializeProxyFromSettings(stringValue(proxyHost, DEFAULT_PROXY_HOST));

        setAutoCacheImages(boolValue(cacheImagesAutomatically, DEFAULT_AUTO_CACHE_IMAGES));

        setArtworkEditRightPaneWidth(intValue(artworkEditRightPaneWidth, DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH));
        setSelectedDictIndex(intValue(translatorSelectedDictIndex, DEFAULT_SELECTED_DICT_INDEX));

        sync();
    }

    void SettingsModel::resetToDefault() {
        LOG_INFO << "#";

        setExifToolPath(DEFAULT_EXIFTOOL);
        setDictionaryPath(DEFAULT_DICT_PATH);
        setUploadTimeout(DEFAULT_UPLOAD_TIMEOUT);
        setMustUseMasterPassword(DEFAULT_USE_MASTERPASSWORD);
        setMustUseConfirmations(DEFAULT_USE_CONFIRMATIONS);
        setSaveBackups(DEFAULT_SAVE_BACKUPS);
        setKeywordSizeScale(DEFAULT_KEYWORD_SIZE_SCALE);
        setDismissDuration(DEFAULT_DISMISS_DURATION);
        setMaxParallelUploads(DEFAULT_MAX_PARALLEL_UPLOADS);
        setFitSmallPreview(DEFAULT_FIT_SMALL_PREVIEW);
        setSearchUsingAnd(DEFAULT_SEARCH_USING_AND);
        setSearchByFilepath(DEFAULT_SEARCH_BY_FILEPATH);
        setScrollSpeedScale(DEFAULT_SCROLL_SPEED_SCALE);
        setUseSpellCheck(DEFAULT_USE_SPELL_CHECK);
        setUserStatistics(DEFAULT_COLLECT_USER_STATISTICS);
        setCheckForUpdates(DEFAULT_CHECK_FOR_UPDATES);
        setAutoDownloadUpdates(DEFAULT_AUTO_DOWNLOAD_UPDATES);
        setAutoFindVectors(DEFAULT_AUTO_FIND_VECTORS);
        setSelectedLocale(DEFAULT_LOCALE);
        setSelectedThemeIndex(DEFAULT_SELECTED_THEME_INDEX);
        setUseAutoComplete(DEFAULT_USE_AUTO_COMPLETE);
        setUseExifTool(DEFAULT_USE_EXIFTOOL);
        setUseProxy(DEFAULT_USE_PROXY);
        resetProxySetting();
        setAutoCacheImages(DEFAULT_AUTO_CACHE_IMAGES);
        setArtworkEditRightPaneWidth(DEFAULT_ARTWORK_EDIT_RIGHT_PANE_WIDTH);
        setSelectedDictIndex(DEFAULT_SELECTED_DICT_INDEX);
        setVerboseUpload(DEFAULT_VERBOSE_UPLOAD);

#if defined(QT_DEBUG)
        setValue(Constants::userConsent, DEFAULT_HAVE_USER_CONSENT);
        setValue(Constants::installedVersion, 0);
#endif

        // resetting position in settings is pretty useless because
        // we will overwrite them on Xpiks exit. But anyway for the future...
        setValue(Constants::appWindowHeight, DEFAULT_APP_HEIGHT);
        setValue(Constants::appWindowWidth, DEFAULT_APP_WIDTH);
        setValue(Constants::appWindowX, DEFAULT_APP_POSITION);
        setValue(Constants::appWindowY, DEFAULT_APP_POSITION);

        sync();
    }

    int ensureInBounds(int value, int boundA, int boundB) {
        Q_ASSERT(boundA <= boundB);

        if (value < boundA) {
            value = boundA;
        }

        if (value > boundB) {
            value = boundB;
        }

        return value;
    }

    double ensureInBounds(double value, double boundA, double boundB) {
        Q_ASSERT(boundA <= boundB);

        // double nan
        if (value != value) {
            value = boundA;
        }

        if (value < boundA) {
            value = boundA;
        }

        if (value > boundB) {
            value = boundB;
        }

        return value;
    }

    void SettingsModel::saveProxySetting(const QString &address, const QString &user,
                                         const QString &password, const QString &port) {
        QString tAddress = address.trimmed();
        QString tUser = user.trimmed();
        QString tPassword = password.trimmed();
        QString tPort = port.trimmed();

        if (tAddress != m_ProxySettings.m_Address) {
            m_ProxySettings.m_Address = tAddress;
            emit proxyAddressChanged(tAddress);
        }

        if (tUser != m_ProxySettings.m_User) {
            m_ProxySettings.m_User = tUser;
            emit proxyUserChanged(tUser);
        }

        if (tPassword != m_ProxySettings.m_Password) {
            m_ProxySettings.m_Password = tPassword;
            emit proxyPasswordChanged(tPassword);
        }

        if (tPort != m_ProxySettings.m_Port) {
            m_ProxySettings.m_Port = tPort;
            emit proxyPortChanged(tPort);
        }
    }

    QString SettingsModel::serializeProxyForSettings(ProxySettings &settings) {
        QByteArray raw;
        QDataStream ds(&raw, QIODevice::WriteOnly);
        ds << settings;
        return QString::fromLatin1(raw.toBase64());
    }

    void SettingsModel::saveArtworkEditUISettings() {
        setValue(Constants::artworkEditRightPaneWidth, m_ArtworkEditRightPaneWidth);
        sync();
    }

    void SettingsModel::saveSelectedDictionaryIndex() {
        setValue(Constants::translatorSelectedDictIndex, m_SelectedDictIndex);
        sync();
    }

    QString SettingsModel::getWhatsNewText() const {
        QString text;
        QString path;

#if !defined(Q_OS_LINUX)
        path = QCoreApplication::applicationDirPath();

#if defined(Q_OS_MAC)
        path += "/../Resources/";
#endif

        path += QDir::separator() + QLatin1String(Constants::WHATS_NEW_FILENAME);
        path = QDir::cleanPath(path);
#else
        path = QStandardPaths::locate(XPIKS_DATA_LOCATION_TYPE, Constants::WHATS_NEW_FILENAME);
#endif
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            text = QString::fromUtf8(file.readAll());
            file.close();
        } else {
            LOG_WARNING << "whatsnew.txt file is not found on path" << path;

            path = QDir::current().absoluteFilePath(QLatin1String(Constants::WHATS_NEW_FILENAME));

            QFile currDirFile(path);
            if (currDirFile.open(QIODevice::ReadOnly)) {
                text = QString::fromUtf8(currDirFile.readAll());
                currDirFile.close();
            }
        }
        return text;
    }

    QString SettingsModel::getTermsAndConditionsText() const {
        QString text;
        QString path;

#if !defined(Q_OS_LINUX)
        path = QCoreApplication::applicationDirPath();

#if defined(Q_OS_MAC)
        path += "/../Resources/";
#endif

        path += QDir::separator() + QLatin1String(Constants::TERMS_AND_CONDITIONS_FILENAME);
        path = QDir::cleanPath(path);
#else
        path = QStandardPaths::locate(XPIKS_DATA_LOCATION_TYPE, Constants::TERMS_AND_CONDITIONS_FILENAME);
#endif
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            text = QString::fromUtf8(file.readAll());
            file.close();
        } else {
            LOG_WARNING << "terms_and_conditions.txt file is not found on path" << path;

            path = QDir::current().absoluteFilePath(QLatin1String(Constants::TERMS_AND_CONDITIONS_FILENAME));

            QFile currDirFile(path);
            if (currDirFile.open(QIODevice::ReadOnly)) {
                text = QString::fromUtf8(currDirFile.readAll());
                currDirFile.close();
            }
        }

        return text;
    }

    void SettingsModel::protectTelemetry() {
        bool telemetryEnabled = this->boolValue(Constants::userStatistics, false);

        if (telemetryEnabled) {
            this->setValue(Constants::numberOfLaunches, 0);
        } else {
            int numberOfLaunches = this->intValue(Constants::numberOfLaunches, 0);
            numberOfLaunches++;

            if (numberOfLaunches >= 31) {
                this->setValue(Constants::userStatistics, true);
                this->setValue(Constants::numberOfLaunches, 0);
                LOG_DEBUG << "Resetting telemetry to ON";
            } else {
                this->setValue(Constants::numberOfLaunches, numberOfLaunches);
                LOG_DEBUG << numberOfLaunches << "launches of Xpiks with Telemetry OFF";
            }
        }
    }

    void SettingsModel::resetProxySetting() {
        QString empty = "";
        SettingsModel::saveProxySetting(empty, empty, empty, empty);
    }
}

