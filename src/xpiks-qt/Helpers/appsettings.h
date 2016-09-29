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

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include "constants.h"
#include "../Common/version.h"
#include "../Common/defines.h"

namespace Helpers {
    class AppSettings : public QSettings
    {
        Q_OBJECT

    public:
        explicit AppSettings(QObject *parent = 0) : QSettings(QSettings::UserScope,
                                                              QCoreApplication::instance()->organizationName(),
                                                              QCoreApplication::instance()->applicationName(),
                                                              parent)
        {
        }

        QString getAppWidthKey()   const { return QLatin1String(Constants::APP_WINDOW_WIDTH); }
        QString getAppHeightKey()  const { return QLatin1String(Constants::APP_WINDOW_HEIGHT); }
        QString getAppPosXKey()    const { return QLatin1String(Constants::APP_WINDOW_X); }
        QString getAppPosYKey()    const { return QLatin1String(Constants::APP_WINDOW_Y); }

        Q_PROPERTY(QString exifToolPathKey READ getExifToolPathKey CONSTANT)
        QString getExifToolPathKey() const { return QLatin1String(Constants::PATH_TO_EXIFTOOL); }

        Q_PROPERTY(QString uploadHostsKey READ getUploadHostsKey CONSTANT)
        QString getUploadHostsKey() const { return QLatin1String(Constants::UPLOAD_HOSTS); }

        Q_PROPERTY(QString minMegapixelCountKey READ getMinMegapixelCountKey CONSTANT)
        QString getMinMegapixelCountKey() const { return QLatin1String(Constants::MIN_MEGAPIXEL_COUNT); }

        Q_PROPERTY(QString maxDescriptionLengthKey READ getMaxDescriptionLengthKey CONSTANT)
        QString getMaxDescriptionLengthKey() const { return QLatin1String(Constants::MAX_DESCRIPTION_LENGTH); }

        Q_PROPERTY(QString maxKeywordsCountKey READ getMaxKeywordsCountKey CONSTANT)
        QString getMaxKeywordsCountKey() const { return QLatin1String(Constants::MAX_KEYWORD_COUNT); }

        Q_PROPERTY(QString appVersion READ getAppVersion CONSTANT)
        QString getAppVersion() const { return QCoreApplication::applicationVersion(); }

        Q_PROPERTY(QString mustUseMasterPasswordKey READ getMustUseMasterPasswordKey CONSTANT)
        QString getMustUseMasterPasswordKey() const { return QLatin1String(Constants::USE_MASTER_PASSWORD); }

        Q_PROPERTY(QString masterPasswordHashKey READ getMasterPasswordHashKey CONSTANT)
        QString getMasterPasswordHashKey() const { return QLatin1String(Constants::MASTER_PASSWORD_HASH); }

        Q_PROPERTY(QString oneUploadMinutesTimeoutKey READ getOneUploadSecondsTimeoutKey CONSTANT)
        QString getOneUploadSecondsTimeoutKey() const { return QLatin1String(Constants::ONE_UPLOAD_SECONDS_TIMEMOUT); }

        Q_PROPERTY(QString useConfirmationDialogsKey READ getUseConfirmationDialogsKey CONSTANT)
        QString getUseConfirmationDialogsKey() const { return QLatin1String(Constants::USE_CONFIRMATION_DIALOGS); }

        Q_PROPERTY(QString saveBackupsKey READ getSaveBackupsKey CONSTANT)
        QString getSaveBackupsKey() const { return QLatin1String(Constants::SAVE_BACKUPS); }

        Q_PROPERTY(QString keywordSizeScale READ getKeywordSizeScaleKey CONSTANT)
        QString getKeywordSizeScaleKey() const { return QLatin1String(Constants::KEYWORD_SIZE_SCALE); }

        Q_PROPERTY(QString recentDirectoriesKey READ getRecentDirectoriesKey CONSTANT)
        QString getRecentDirectoriesKey() const { return QLatin1String(Constants::RECENT_DIRECTORIES); }

        Q_PROPERTY(QString dismissDurationKey READ getDismissDurationKey CONSTANT)
        QString getDismissDurationKey() const { return QLatin1String(Constants::DISMISS_DURATION); }

        Q_PROPERTY(QString maxParallelUploadsKey READ getMaxParallelUploadsKey CONSTANT)
        QString getMaxParallelUploadsKey() const { return QLatin1String(Constants::MAX_PARALLEL_UPLOADS); }

        Q_PROPERTY(QString fitSmallPreviewKey READ getFitSmallPreviewKey CONSTANT)
        QString getFitSmallPreviewKey() const { return QLatin1String(Constants::FIT_SMALL_PREVIEW); }

        Q_PROPERTY(QString searchUsingAndKey READ getSearchUsingAndKey CONSTANT)
        QString getSearchUsingAndKey() const { return QLatin1String(Constants::SEARCH_USING_AND); }

        Q_PROPERTY(QString scrollSpeedScaleKey READ getScrollSpeedScaleKey CONSTANT)
        QString getScrollSpeedScaleKey() const { return QLatin1String(Constants::SCROLL_SPEED_SENSIVITY); }

        Q_PROPERTY(QString autoSpellCheckKey READ getUseSpellCheckKey CONSTANT)
        QString getUseSpellCheckKey() const { return QLatin1String(Constants::USE_SPELL_CHECK); }

        Q_PROPERTY(QString installedVersionKey READ getInstalledVersionKey CONSTANT)
        QString getInstalledVersionKey() const { return QLatin1String(Constants::INSTALLED_VERSION); }

        Q_PROPERTY(QString userConsentKey READ getUserConsentKey CONSTANT)
        QString getUserConsentKey() const { return QLatin1String(Constants::USER_CONSENT); }

        Q_PROPERTY(QString dictionaryPathKey READ getDictionaryPathKey CONSTANT)
        QString getDictionaryPathKey() const { return QLatin1String(Constants::DICT_PATH); }

        Q_PROPERTY(QString userStatisticsKey READ getUserStatisticsKey CONSTANT)
        QString getUserStatisticsKey() const { return QLatin1String(Constants::USER_STATISTICS); }

        Q_PROPERTY(QString checkForUpdatesKey READ getCheckForUpdatesKey CONSTANT)
        QString getCheckForUpdatesKey() const { return QLatin1String(Constants::CHECK_FOR_UPDATES); }

        Q_PROPERTY(QString numberOfLaunchesKey READ getNumberOfLaunchesKey CONSTANT)
        QString getNumberOfLaunchesKey() const { return QLatin1String(Constants::NUMBER_OF_LAUNCHES); }

        Q_PROPERTY(QString autoFindVectorsKey READ getAutoFindVectorsKey CONSTANT)
        QString getAutoFindVectorsKey() const { return QLatin1String(Constants::AUTO_FIND_VECTORS); }

        Q_PROPERTY(QString selectedLocaleKey READ getSelectedLocaleKey CONSTANT)
        QString getSelectedLocaleKey() const { return QLatin1String(Constants::SELECTED_LOCALE); }

        Q_PROPERTY(QString selectedThemeIndexKey READ getSelectedThemeIndexKey CONSTANT)
        QString getSelectedThemeIndexKey() const { return QLatin1String(Constants::SELECTED_THEME_INDEX); }

        Q_PROPERTY(QString useAutoCompleteKey READ getUseAutoCompleteKey CONSTANT)
        QString getUseAutoCompleteKey() const { return QLatin1String(Constants::USE_AUTO_COMPLETE); }

        Q_PROPERTY(QString useExifToolKey READ getUseExifToolKey CONSTANT)
        QString getUseExifToolKey() const { return QLatin1String(Constants::USE_EXIFTOOL); }

        Q_PROPERTY(QString useProxyKey READ getUseProxyKey CONSTANT)
        QString getUseProxyKey() const { return QLatin1String(Constants::USE_PROXY); }

        Q_PROPERTY(QString proxyHashKey READ getproxyHashKey CONSTANT)
        QString getproxyHashKey() const { return QLatin1String(Constants::PROXY_HOST); }

        Q_PROPERTY(QString cacheImagesKey READ getCacheImagesKey CONSTANT)
        QString getCacheImagesKey() const { return QLatin1String(Constants::CACHE_IMAGES_AUTOMATICALLY); }

        Q_PROPERTY(QString autoDownloadUpdatesKey READ getAutoDownloadUpdatesKey CONSTANT)
        QString getAutoDownloadUpdatesKey() const { return QLatin1String(Constants::AUTO_DOWNLOAD_UPDATES); }

        Q_PROPERTY(QString pathToUpdateKey READ getPathToUpdateKey CONSTANT)
        QString getPathToUpdateKey() const { return QLatin1String(Constants::PATH_TO_UPDATE); }

        Q_PROPERTY(QString availableUpdateVersionKey READ getAvailableUpdateVersionKey CONSTANT)
        QString getAvailableUpdateVersionKey() const { return QLatin1String(Constants::AVAILABLE_UPDATE_VERSION); }

        Q_PROPERTY(QString whatsNewText READ getWhatsNewText CONSTANT)
        QString getWhatsNewText() const {
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

        Q_INVOKABLE inline void setValue(const QString &key, const QVariant &value) {
            QSettings::setValue(key, value);
        }

        Q_INVOKABLE inline QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const {
            return QSettings::value(key, defaultValue);
        }

        Q_INVOKABLE inline bool boolValue(const QString &key, const QVariant &defaultValue = QVariant()) const {
            return QSettings::value(key, defaultValue).toBool();
        }

        Q_INVOKABLE inline double doubleValue(const QString &key, const QVariant &defaultValue = QVariant()) const {
            return QSettings::value(key, defaultValue).toDouble();
        }

        Q_INVOKABLE inline int intValue(const QString &key, const QVariant &defaultValue = QVariant()) const {
            return QSettings::value(key, defaultValue).toInt();
        }

        Q_INVOKABLE bool needToShowWhatsNew() {
            int lastVersion = intValue(getInstalledVersionKey(), 0);
            int installedMajorPart = lastVersion / 10;
            int currentMajorPart = XPIKS_VERSION_INT / 10;
            bool result = currentMajorPart > installedMajorPart;
            return result;
        }

        Q_INVOKABLE bool needToShowTextWhatsNew() {
            int lastVersion = intValue(getInstalledVersionKey(), 0);
            int installedMajorPart = lastVersion / 10;
            int currentMajorPart = XPIKS_VERSION_INT / 10;
            bool result = (currentMajorPart == installedMajorPart) &&
                    (XPIKS_VERSION_INT > lastVersion);
            return result;
        }

        Q_INVOKABLE void saveCurrentVersion() {
            LOG_DEBUG << "Saving current xpiks version";
            setValue(getInstalledVersionKey(), XPIKS_VERSION_INT);
        }

        Q_INVOKABLE bool needToShowTermsAndConditions() {
            bool haveConsent = boolValue(getUserConsentKey(), false);
            return !haveConsent;
        }

        Q_INVOKABLE void userAgreeHandler() {
            setValue(getUserConsentKey(), true);
        }

        Q_INVOKABLE int getAppWidth(int defaultWidth) {
            return intValue(getAppWidthKey(), defaultWidth);
        }

        Q_INVOKABLE void setAppWidth(int width) {
            setValue(getAppWidthKey(), width);
        }

        Q_INVOKABLE int getAppHeight(int defaultHeight) {
            return intValue(getAppHeightKey(), defaultHeight);
        }

        Q_INVOKABLE void setAppHeight(int height) {
            setValue(getAppHeightKey(), height);
        }

        Q_INVOKABLE int getAppPosX(int defaultPosX) {
            int posX = intValue(getAppPosXKey(), defaultPosX);
            if (posX == -1) { posX = defaultPosX; }
            return posX;
        }

        Q_INVOKABLE void setAppPosX(int x) {
            setValue(getAppPosXKey(), x);
        }

        Q_INVOKABLE int getAppPosY(int defaultPosY) {
            int posY = intValue(getAppPosYKey(), defaultPosY);
            if (posY == -1) { posY = defaultPosY; }
            return posY;
        }

        Q_INVOKABLE void setAppPosY(int y) {
            setValue(getAppPosYKey(), y);
        }

        Q_INVOKABLE void protectTelemetry() {
            bool telemetryEnabled = this->value(Constants::USER_STATISTICS, false).toBool();

            if (telemetryEnabled) {
                this->setValue(Constants::NUMBER_OF_LAUNCHES, 0);
            } else {
                int numberOfLaunches = this->value(Constants::NUMBER_OF_LAUNCHES, 0).toInt();
                numberOfLaunches++;

                if (numberOfLaunches >= 31) {
                    this->setValue(Constants::USER_STATISTICS, true);
                    this->setValue(Constants::NUMBER_OF_LAUNCHES, 0);
                    LOG_DEBUG << "Resetting telemetry to ON";
                } else {
                    this->setValue(Constants::NUMBER_OF_LAUNCHES, numberOfLaunches);
                    LOG_DEBUG << numberOfLaunches << "launches of Xpiks with Telemetry OFF";
                }
            }
        }

        Q_PROPERTY(QString termsAndConditionsText READ getTermsAndConditionsText CONSTANT)
        QString getTermsAndConditionsText() const {
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
    };
}

Q_DECLARE_METATYPE(Helpers::AppSettings*)

#endif // APPSETTINGS_H
