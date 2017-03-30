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

#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QJsonObject>
#include "../Common/baseentity.h"
#include "../Common/version.h"
#include "../Models/proxysettings.h"
#include "../Helpers/localconfig.h"
#include "../Helpers/constants.h"
#include "../Commands/commandmanager.h"
#include "../Encryption/secretsmanager.h"
#include "../Models/recentdirectoriesmodel.h"
#include "../Models/recentfilesmodel.h"
#include "../Models/uploadinforepository.h"

#define SETTINGS_EPSILON 1e-9

namespace Models {

    int ensureInBounds(int value, int boundA, int boundB);
    double ensureInBounds(double value, double boundA, double boundB);

    class SettingsModel : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString exifToolPath READ getExifToolPath WRITE setExifToolPath NOTIFY exifToolPathChanged)
        Q_PROPERTY(int uploadTimeout READ getUploadTimeout WRITE setUploadTimeout NOTIFY uploadTimeoutChanged)
        Q_PROPERTY(bool mustUseMasterPassword READ getMustUseMasterPassword WRITE setMustUseMasterPassword NOTIFY mustUseMasterPasswordChanged)
        Q_PROPERTY(bool mustUseConfirmations READ getMustUseConfirmations WRITE setMustUseConfirmations NOTIFY mustUseConfirmationsChanged)
        Q_PROPERTY(bool saveBackups READ getSaveBackups WRITE setSaveBackups NOTIFY saveBackupsChanged)
        Q_PROPERTY(double keywordSizeScale READ getKeywordSizeScale WRITE setKeywordSizeScale NOTIFY keywordSizeScaleChanged)
        Q_PROPERTY(int dismissDuration READ getDismissDuration WRITE setDismissDuration NOTIFY dismissDurationChanged)
        Q_PROPERTY(int maxParallelUploads READ getMaxParallelUploads WRITE setMaxParallelUploads NOTIFY maxParallelUploadsChanged)
        Q_PROPERTY(bool fitSmallPreview READ getFitSmallPreview WRITE setFitSmallPreview NOTIFY fitSmallPreviewChanged)
        Q_PROPERTY(bool searchUsingAnd READ getSearchUsingAnd WRITE setSearchUsingAnd NOTIFY searchUsingAndChanged)
        Q_PROPERTY(bool searchByFilepath READ getSearchByFilepath WRITE setSearchByFilepath NOTIFY searchByFilepathChanged)
        Q_PROPERTY(double scrollSpeedScale READ getScrollSpeedScale WRITE setScrollSpeedScale NOTIFY scrollSpeedScaleChanged)
        Q_PROPERTY(bool useSpellCheck READ getUseSpellCheck WRITE setUseSpellCheck NOTIFY useSpellCheckChanged)
        Q_PROPERTY(bool userStatistics READ getUserStatistics WRITE setUserStatistics NOTIFY userStatisticsChanged)
        Q_PROPERTY(bool checkForUpdates READ getCheckForUpdates WRITE setCheckForUpdates NOTIFY checkForUpdatesChanged)
        Q_PROPERTY(bool autoDownloadUpdates READ getAutoDownloadUpdates WRITE setAutoDownloadUpdates NOTIFY autoDownloadUpdatesChanged)
        Q_PROPERTY(QString dictionaryPath READ getDictionaryPath WRITE setDictionaryPath NOTIFY dictionaryPathChanged)
        Q_PROPERTY(bool autoFindVectors READ getAutoFindVectors WRITE setAutoFindVectors NOTIFY autoFindVectorsChanged)
        Q_PROPERTY(QString selectedLocale READ getSelectedLocale WRITE setSelectedLocale NOTIFY selectedLocaleChanged)
        Q_PROPERTY(int selectedThemeIndex READ getSelectedThemeIndex WRITE setSelectedThemeIndex NOTIFY selectedThemeIndexChanged)
        Q_PROPERTY(bool useAutoComplete READ getUseAutoComplete WRITE setUseAutoComplete NOTIFY useAutoCompleteChanged)
        Q_PROPERTY(bool useExifTool READ getUseExifTool WRITE setUseExifTool NOTIFY useExifToolChanged)
        Q_PROPERTY(bool useProxy READ getUseProxy WRITE setUseProxy NOTIFY useProxyChanged)
        Q_PROPERTY(QString proxyAddress READ getProxyAddress NOTIFY proxyAddressChanged)
        Q_PROPERTY(QString proxyUser READ getProxyUser NOTIFY proxyUserChanged)
        Q_PROPERTY(QString proxyPassword READ getProxyPassword NOTIFY proxyPasswordChanged)
        Q_PROPERTY(QString proxyPort READ getProxyPort NOTIFY proxyPortChanged)
        Q_PROPERTY(bool autoCacheImages READ getAutoCacheImages WRITE setAutoCacheImages NOTIFY autoCacheImagesChanged)
        Q_PROPERTY(int artworkEditRightPaneWidth READ getArtworkEditRightPaneWidth WRITE setArtworkEditRightPaneWidth NOTIFY artworkEditRightPaneWidthChanged)
        Q_PROPERTY(bool verboseUpload READ getVerboseUpload WRITE setVerboseUpload NOTIFY verboseUploadChanged)

        Q_PROPERTY(QString appVersion READ getAppVersion CONSTANT)

        Q_PROPERTY(QString whatsNewText READ getWhatsNewText CONSTANT)
        Q_PROPERTY(QString termsAndConditionsText READ getTermsAndConditionsText CONSTANT)

    public:
        explicit SettingsModel(QObject *parent = 0);
        virtual ~SettingsModel() { sync(); }

    public:
        void saveExiftool();
        void saveLocale();
        void initializeConfigs();
        void syncronizeSettings() { sync(); }

    public:
        ProxySettings *retrieveProxySettings();
        bool getIsTelemetryEnabled() const { return boolValue(Constants::userStatistics, true); }

    public:
        Q_INVOKABLE void resetAllValues();
        Q_INVOKABLE void saveAllValues();
        Q_INVOKABLE void clearMasterPasswordSettings();
        Q_INVOKABLE void resetExifTool();
        Q_INVOKABLE void resetDictPath();
        Q_INVOKABLE void readAllValues();
        Q_INVOKABLE void raiseMasterPasswordSignal() { emit mustUseMasterPasswordChanged(m_MustUseMasterPassword); }
        Q_INVOKABLE void saveProxySetting(const QString &address, const QString &user, const QString &password, const QString &port);
        Q_INVOKABLE void saveArtworkEditUISettings();
        Q_INVOKABLE void saveSelectedDictionaryIndex();

    private:
        inline void setValue(const char *key, const QJsonValue &value) {
            m_SettingsJson.insert(QLatin1String(key), value);
        }

        inline QJsonValue value(const char *key, const QJsonValue &defaultValue = QJsonValue()) const {
            QJsonValue value = m_SettingsJson.value(QLatin1String(key));

            if (value.isUndefined()) {
                return defaultValue;
            }

            return value;
        }

        inline bool boolValue(const char *key, const bool defaultValue = false) const {
            return m_SettingsJson.value(QLatin1String(key)).toBool(defaultValue);
        }

        inline double doubleValue(const char *key, const double defaultValue = 0) const {
            return m_SettingsJson.value(QLatin1String(key)).toDouble(defaultValue);
        }

        inline int intValue(const char *key, const int defaultValue = 0) const {
            return m_SettingsJson.value(QLatin1String(key)).toInt(defaultValue);
        }

        inline QString stringValue(const char *key, const QString defaultValue = QString("")) const {
            return m_SettingsJson.value(QLatin1String(key)).toString(defaultValue);
        }

    private:
        QString getAppVersion() const { return QCoreApplication::applicationVersion(); }
        QString getWhatsNewText() const;
        QString getTermsAndConditionsText() const;

    public:
        Q_INVOKABLE QString getRecentDirectories() { return stringValue(Constants::recentDirectories); }
        Q_INVOKABLE QString getRecentFiles() { return stringValue(Constants::recentFiles); }
        Q_INVOKABLE QString getUserAgentId() { return stringValue(Constants::userAgentId); }
        Q_INVOKABLE QString getPathToUpdate() { return stringValue(Constants::pathToUpdate); }
        Q_INVOKABLE QString getUploadHosts() { return stringValue(Constants::uploadHosts); }
        Q_INVOKABLE QString getMasterPasswordHash() { return stringValue(Constants::masterPasswordHash); }
        Q_INVOKABLE QString getDictPath() { return stringValue(Constants::dictPath); }
        Q_INVOKABLE bool getMustUseConfirmationDialogs() { return boolValue(Constants::useConfirmationDialogs, true); }
        Q_INVOKABLE int getAvailableUpdateVersion() { return intValue(Constants::availableUpdateVersion); }

    public:
        Q_INVOKABLE bool needToShowWhatsNew() {
            int lastVersion = intValue(Constants::installedVersion, 0);
            int installedMajorPart = lastVersion / 10;
            int currentMajorPart = XPIKS_VERSION_INT / 10;
            bool result = currentMajorPart > installedMajorPart;
            return result;
        }

        Q_INVOKABLE bool needToShowTextWhatsNew() {
            int lastVersion = intValue(Constants::installedVersion, 0);
            int installedMajorPart = lastVersion / 10;
            int currentMajorPart = XPIKS_VERSION_INT / 10;
            bool result = (currentMajorPart == installedMajorPart) &&
                    (XPIKS_VERSION_INT > lastVersion);
            return result;
        }

        Q_INVOKABLE void saveCurrentVersion() {
            LOG_DEBUG << "Saving current xpiks version";
            setValue(Constants::installedVersion, XPIKS_VERSION_INT);
        }

        Q_INVOKABLE bool needToShowTermsAndConditions() {
            bool haveConsent = boolValue(Constants::userConsent, false);
            return !haveConsent;
        }

        Q_INVOKABLE void userAgreeHandler() {
            LOG_DEBUG << "#";
            setValue(Constants::userConsent, true);
        }

        Q_INVOKABLE int getAppWidth(int defaultWidth) {
            return intValue(Constants::appWindowWidth, defaultWidth);
        }

        Q_INVOKABLE void setAppWidth(int width) {
            LOG_DEBUG << "#";

            setValue(Constants::appWindowWidth, width);
        }

        Q_INVOKABLE int getAppHeight(int defaultHeight) {
            return intValue(Constants::appWindowHeight, defaultHeight);
        }

        Q_INVOKABLE void setAppHeight(int height) {
            LOG_DEBUG << "#";

            setValue(Constants::appWindowHeight, height);
        }

        Q_INVOKABLE int getAppPosX(int defaultPosX) {
            int posX = intValue(Constants::appWindowX, defaultPosX);
            if (posX == -1) { posX = defaultPosX; }
            return posX;
        }

        Q_INVOKABLE void setAppPosX(int x) {
            LOG_DEBUG << "#";

            setValue(Constants::appWindowX, x);
        }

        Q_INVOKABLE int getAppPosY(int defaultPosY) {
            int posY = intValue(Constants::appWindowY, defaultPosY);
            if (posY == -1) { posY = defaultPosY; }
            return posY;
        }

        Q_INVOKABLE void setAppPosY(int y) {
            LOG_DEBUG << "#";
            setValue(Constants::appWindowY, y);
        }

        Q_INVOKABLE void setUseMasterPassword(bool value) {
            LOG_DEBUG << "#";
            setValue(Constants::useMasterPassword, value);
        }

        Q_INVOKABLE void setMasterPasswordHash() {
            LOG_DEBUG << "#";
            Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();
            setValue(Constants::masterPasswordHash, secretsManager->getMasterPasswordHash());
        }

        Q_INVOKABLE void saveUploadHosts() {
            LOG_DEBUG << "#";
            UploadInfoRepository *uploadInfoRepository = m_CommandManager->getUploadInfoRepository();
            setValue(Constants::uploadHosts, uploadInfoRepository->getInfoString());
        }

        Q_INVOKABLE void saveRecentDirectories() {
            LOG_DEBUG << "#";
            Models::RecentDirectoriesModel *recentDirectories = m_CommandManager->getRecentDirectories();
            setValue(Constants::recentDirectories, recentDirectories->serializeForSettings());
        }

        Q_INVOKABLE void saveRecentFiles() {
            LOG_DEBUG << "#";
            Models::RecentFilesModel *recentFiles = m_CommandManager->getRecentFiles();
            setValue(Constants::recentFiles, recentFiles->serializeForSettings());
        }

        Q_INVOKABLE void setUserAgentId(const QString &id) {
            LOG_DEBUG << "#";
            setValue(Constants::userAgentId, id);
        }

        Q_INVOKABLE void setAvailableUpdateVersion(int version) {
            LOG_DEBUG << "#";
            setValue(Constants::availableUpdateVersion, version);
        }

        Q_INVOKABLE void setPathToUpdate(QString path) {
            LOG_DEBUG << "#";
            setValue(Constants::pathToUpdate, path);
        }

        Q_INVOKABLE void protectTelemetry();

        Q_INVOKABLE void onMasterPasswordSet() {
            LOG_INFO << "Master password changed";

            setMasterPasswordHash();
            setUseMasterPassword(true);
            setMustUseMasterPassword(true);
        }

        Q_INVOKABLE void onMasterPasswordUnset(bool firstTime) {
            setMustUseMasterPassword(!firstTime);
            raiseMasterPasswordSignal();
        }

    public:
        QString getExifToolPath() const { return m_ExifToolPath; }
        int getUploadTimeout() const { return m_UploadTimeout; }
        bool getMustUseMasterPassword() const { return m_MustUseMasterPassword; }
        bool getMustUseConfirmations() const { return m_MustUseConfirmations; }
        bool getSaveBackups() const { return m_SaveBackups; }
        double getKeywordSizeScale() const { return m_KeywordSizeScale; }
        int getDismissDuration() const { return m_DismissDuration; }
        int getMaxParallelUploads() const { return m_MaxParallelUploads; }
        bool getFitSmallPreview() const { return m_FitSmallPreview; }
        bool getSearchUsingAnd() const { return m_SearchUsingAnd; }
        bool getSearchByFilepath() const { return m_SearchByFilepath; }
        double getScrollSpeedScale() const { return m_ScrollSpeedScale; }
        bool getUseSpellCheck() const { return m_UseSpellCheck; }
        bool getUserStatistics() const { return m_UserStatistics; }
        bool getCheckForUpdates() const { return m_CheckForUpdates; }
        bool getAutoDownloadUpdates() const { return m_AutoDownloadUpdates; }
        QString getDictionaryPath() const { return m_DictPath; }
        bool getAutoFindVectors() const { return m_AutoFindVectors; }
        QString getSelectedLocale() const { return m_SelectedLocale; }
        int getSelectedThemeIndex() const { return m_SelectedThemeIndex; }
        bool getUseAutoComplete() const { return m_UseAutoComplete; }
        bool getUseExifTool() const { return m_UseExifTool; }
        bool getUseProxy() const { return m_UseProxy; }
        QString getProxyAddress() const { return m_ProxySettings.m_Address; }
        QString getProxyUser() const { return m_ProxySettings.m_User; }
        QString getProxyPassword() const { return m_ProxySettings.m_Password; }
        QString getProxyPort() const { return m_ProxySettings.m_Port; }
        ProxySettings *getProxySettings() { return &m_ProxySettings; }
        bool getAutoCacheImages() const { return m_AutoCacheImages; }
        int getArtworkEditRightPaneWidth() const { return m_ArtworkEditRightPaneWidth; }
        int getSelectedDictIndex() const { return m_SelectedDictIndex; }
        bool getVerboseUpload() const { return m_VerboseUpload; }

    signals:
        void settingsReset();
        void settingsUpdated();
        void exifToolPathChanged(QString exifToolPath);
        void uploadTimeoutChanged(int uploadTimeout);
        void mustUseMasterPasswordChanged(bool mustUseMasterPassword);
        void mustUseConfirmationsChanged(bool mustUseConfirmations);
        void saveBackupsChanged(bool saveBackups);
        void keywordSizeScaleChanged(double value);
        void dismissDurationChanged(int value);
        void maxParallelUploadsChanged(int value);
        void fitSmallPreviewChanged(bool value);
        void searchUsingAndChanged(bool value);
        void searchByFilepathChanged(bool value);
        void scrollSpeedScaleChanged(double value);
        void useSpellCheckChanged(bool value);
        void userStatisticsChanged(bool value);
        void checkForUpdatesChanged(bool value);
        void autoDownloadUpdatesChanged(bool value);
        void dictionaryPathChanged(QString path);
        void autoFindVectorsChanged(bool value);
        void selectedLocaleChanged(QString value);
        void selectedThemeIndexChanged(int value);
        void useAutoCompleteChanged(bool value);
        void useExifToolChanged(bool value);
        void useProxyChanged(bool value);
        void proxyAddressChanged(QString value);
        void proxyUserChanged(QString value);
        void proxyPasswordChanged(QString value);
        void proxyPortChanged(QString value);
        void autoCacheImagesChanged(bool value);
        void artworkEditRightPaneWidthChanged(int value);
        void selectedDictIndexChanged(int value);        
        void verboseUploadChanged(bool verboseUpload);

    public:
        void setExifToolPath(QString exifToolPath) {
            if (m_ExifToolPath == exifToolPath)
                return;

            m_ExifToolPath = exifToolPath;
            emit exifToolPathChanged(exifToolPath);
        }

        void setUploadTimeout(int uploadTimeout) {
            if (m_UploadTimeout == uploadTimeout)
                return;

            m_UploadTimeout = ensureInBounds(uploadTimeout, 1, 300);
            emit uploadTimeoutChanged(m_UploadTimeout);
        }

        void setMustUseMasterPassword(bool mustUseMasterPassword) {
            if (m_MustUseMasterPassword == mustUseMasterPassword)
                return;

            m_MustUseMasterPassword = mustUseMasterPassword;
            emit mustUseMasterPasswordChanged(mustUseMasterPassword);
        }

        void setMustUseConfirmations(bool mustUseConfirmations) {
            if (m_MustUseConfirmations == mustUseConfirmations)
                return;

            m_MustUseConfirmations = mustUseConfirmations;
            emit mustUseConfirmationsChanged(mustUseConfirmations);
        }

        void setSaveBackups(bool saveBackups) {
            if (m_SaveBackups == saveBackups)
                return;

            m_SaveBackups = saveBackups;
            emit saveBackupsChanged(saveBackups);
        }

        void setKeywordSizeScale(double value) {
            if (qAbs(m_KeywordSizeScale - value) <= SETTINGS_EPSILON)
                return;

            m_KeywordSizeScale = ensureInBounds(value, 1.0, 1.2);
            emit keywordSizeScaleChanged(m_KeywordSizeScale);
        }

        void setDismissDuration(int value) {
            if (m_DismissDuration == value)
                return;

            m_DismissDuration = ensureInBounds(value, 1, 100);
            emit dismissDurationChanged(m_DismissDuration);
        }

        void setMaxParallelUploads(int value) {
            if (m_MaxParallelUploads == value)
                return;

            m_MaxParallelUploads = ensureInBounds(value, 1, 4);
            emit maxParallelUploadsChanged(m_MaxParallelUploads);
        }

        void setFitSmallPreview(bool value) {
            if (m_FitSmallPreview == value)
                return;

            m_FitSmallPreview = value;
            emit fitSmallPreviewChanged(value);
        }

        void setSearchUsingAnd(bool value) {
            if (m_SearchUsingAnd == value)
                return;

            m_SearchUsingAnd = value;
            emit searchUsingAndChanged(value);
        }

        void setSearchByFilepath(bool value) {
            if (m_SearchByFilepath == value)
                return;

            m_SearchByFilepath = value;
            emit searchByFilepathChanged(value);
        }

        void setScrollSpeedScale(double value) {
            if (qAbs(m_ScrollSpeedScale - value) <= SETTINGS_EPSILON)
                return;

            m_ScrollSpeedScale = ensureInBounds(value, 0.1, 2.0);
            emit scrollSpeedScaleChanged(m_ScrollSpeedScale);
        }

        void setUseSpellCheck(bool value) {
            if (m_UseSpellCheck == value)
                return;

            m_UseSpellCheck = value;
            emit useSpellCheckChanged(value);
        }

        void setUserStatistics(bool value) {
            if (m_UserStatistics == value)
                return;

            m_UserStatistics = value;
            emit userStatisticsChanged(value);
        }

        void setCheckForUpdates(bool value) {
            if (m_CheckForUpdates == value)
                return;

            m_CheckForUpdates = value;
            emit checkForUpdatesChanged(value);
        }

        void setAutoDownloadUpdates(bool value) {
            if (m_AutoDownloadUpdates == value)
                return;

            m_AutoDownloadUpdates = value;
            emit autoDownloadUpdatesChanged(value);
        }

        void setDictionaryPath(QString path) {
            if (m_DictPath == path)
                return;

            m_DictPath = path;
            emit dictionaryPathChanged(path);
            m_DictsPathChanged = true;
        }

        void setAutoFindVectors(bool value) {
            if (value != m_AutoFindVectors) {
                m_AutoFindVectors = value;
                emit autoFindVectorsChanged(value);
            }
        }

        void setSelectedLocale(QString value) {
            if (value != m_SelectedLocale) {
                m_SelectedLocale = value;
                emit selectedLocaleChanged(value);
            }
        }

        void setSelectedThemeIndex(int value) {
            if (value != m_SelectedThemeIndex) {
                m_SelectedThemeIndex = value;
                emit selectedThemeIndexChanged(value);
            }
        }

        void setUseAutoComplete(bool value) {
            if (value != m_UseAutoComplete) {
                m_UseAutoComplete = value;
                emit useAutoCompleteChanged(value);
            }
        }
        void setUseProxy(bool value) {
            if (value != m_UseProxy) {
                m_UseProxy = value;
                emit useProxyChanged(value);
            }
         }

        void setUseExifTool(bool value) {
            if (value != m_UseExifTool) {
                m_UseExifTool = value;
                emit useExifToolChanged(value);
            }
        }

        void setAutoCacheImages(bool value) {
            if (value != m_AutoCacheImages) {
                m_AutoCacheImages = value;
                emit autoCacheImagesChanged(value);
            }
        }

        void setArtworkEditRightPaneWidth(int value) {
            if (value != m_ArtworkEditRightPaneWidth) {
                m_ArtworkEditRightPaneWidth = value;
                emit artworkEditRightPaneWidthChanged(value);
            }
        }

        void setSelectedDictIndex(int value) {
            if (value != m_SelectedDictIndex) {
                m_SelectedDictIndex = value;
                emit selectedDictIndexChanged(value);
            }
        }

        void setVerboseUpload(bool verboseUpload)
        {
            if (m_VerboseUpload == verboseUpload)
                return;

            m_VerboseUpload = verboseUpload;
            emit verboseUploadChanged(verboseUpload);
        }

    public:

#ifndef INTEGRATION_TESTS
    private:
#else
    public:
#endif
        void resetToDefault();

    private:
        void resetProxySetting();

    private:
        void sync();
        QString serializeProxyForSettings(ProxySettings &settings);
        void deserializeProxyFromSettings(const QString &serialized);

    private:
        Helpers::LocalConfig m_Config;
        QJsonObject m_SettingsJson;
        QString m_ExifToolPath;
        QString m_DictPath;
        QString m_SelectedLocale;
        double m_KeywordSizeScale;
        double m_ScrollSpeedScale;
        int m_ArtworkEditRightPaneWidth;
        int m_UploadTimeout; // in seconds
        int m_DismissDuration;
        int m_MaxParallelUploads;
        int m_SelectedThemeIndex;
        int m_SelectedDictIndex;
        bool m_MustUseMasterPassword;
        bool m_MustUseConfirmations;
        bool m_SaveBackups;
        bool m_FitSmallPreview;
        bool m_SearchUsingAnd;
        bool m_SearchByFilepath;
        bool m_UseSpellCheck;
        bool m_UserStatistics;
        bool m_CheckForUpdates;
        bool m_AutoDownloadUpdates;
        bool m_DictsPathChanged;
        bool m_AutoFindVectors;
        bool m_UseAutoComplete;
        bool m_UseExifTool;
        bool m_UseProxy;
        ProxySettings m_ProxySettings;
        bool m_AutoCacheImages;
        bool m_VerboseUpload;
    };
}

#endif // SETTINGSMODEL_H
