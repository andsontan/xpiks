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

#include <QDebug>
#include <QString>
#include <QSettings>
#include <QCoreApplication>
#include <QStandardPaths>
#include "constants.h"
#include "../Common/version.h"
#include <QFile>
#include <QDir>

namespace Helpers {
    class AppSettings : public QSettings
    {
        Q_OBJECT

    public:
        QStandardPaths::StandardLocation appDataLocationType;

        explicit AppSettings(QObject *parent = 0) : QSettings(QSettings::UserScope,
                                                              QCoreApplication::instance()->organizationName(),
                                                              QCoreApplication::instance()->applicationName(),
                                                              parent)
        {
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
                appDataLocationType = QStandardPaths::AppDataLocation;
            #else
                appDataLocationType = QStandardPaths::DataLocation;
            #endif
            //qDebug()<< "Extra files search locations: "<<QStandardPaths::standardLocations(appDataLocationType);
        }

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

        Q_PROPERTY(QString oneUploadMinutesTimeoutKey READ getOneUploadMinutesTimeoutKey CONSTANT)
        QString getOneUploadMinutesTimeoutKey() const { return QLatin1String(Constants::ONE_UPLOAD_MINUTES_TIMEMOUT); }

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
        QString getScrollSpeedScaleKey() const { return QLatin1String(Constants::SCROLL_SPEED_SCALE); }

        Q_PROPERTY(QString autoSpellCheckKey READ getUseSpellCheckKey CONSTANT)
        QString getUseSpellCheckKey() const { return QLatin1String(Constants::USE_SPELL_CHECK); }

        Q_PROPERTY(QString installedVersionKey READ getInstalledVersionKey CONSTANT)
        QString getInstalledVersionKey() const { return QLatin1String(Constants::INSTALLED_VERSION); }

        Q_PROPERTY(QString userConsentKey READ getUserConsentKey CONSTANT)
        QString getUserConsentKey() const { return QLatin1String(Constants::USER_CONSENT); }

        Q_PROPERTY(QString dictionaryPathKey READ getDictionaryPathKey CONSTANT)
        QString getDictionaryPathKey() const { return QLatin1String(Constants::DICT_PATH); }

        Q_PROPERTY(QString userStatisticKey READ getUserStatisticKey CONSTANT)
        QString getUserStatisticKey() const { return QLatin1String(Constants::USER_STATISTIC); }

        Q_PROPERTY(QString numberOfLaunchesKey READ getNumberOfLaunchesKey CONSTANT)
        QString getNumberOfLaunchesKey() const { return QLatin1String(Constants::NUMBER_OF_LAUNCHES); }

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
            path = QStandardPaths::locate(appDataLocationType, Constants::WHATS_NEW_FILENAME);
#endif
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                text = file.readAll();
                file.close();
            } else {
                qWarning() << "whatsnew.txt file is not found on path" << path;

                path = QDir::current().absoluteFilePath(QLatin1String(Constants::WHATS_NEW_FILENAME));

                QFile currDirFile(path);
                if (currDirFile.open(QIODevice::ReadOnly)) {
                    text = currDirFile.readAll();
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
            bool result = XPIKS_VERSION_INT > lastVersion;
            return result;
        }

        Q_INVOKABLE void saveCurrentVersion() {
            qDebug() << "Saving current xpiks version";
            setValue(getInstalledVersionKey(), XPIKS_VERSION_INT);
        }

        Q_INVOKABLE bool needToShowTermsAndConditions() {
            bool haveConsent = boolValue(getUserConsentKey(), false);
            return !haveConsent;
        }

        Q_INVOKABLE void userAgreeHandler() {
            setValue(getUserConsentKey(), true);
        }

        Q_INVOKABLE void protectTelemetry() {
            bool telemetryEnabled = this->value(Constants::USER_STATISTIC, false).toBool();
            if (telemetryEnabled) {
                this->setValue(Constants::NUMBER_OF_LAUNCHES, 0);
            } else {
                int numberOfLaunches = this->value(Constants::NUMBER_OF_LAUNCHES, 0).toInt();
                numberOfLaunches++;

                if (numberOfLaunches >= 31) {
                    this->setValue(Constants::USER_STATISTIC, true);
                    this->setValue(Constants::NUMBER_OF_LAUNCHES, 0);
                    qDebug() << "Resetting telemetry to ON";
                } else {
                    this->setValue(Constants::NUMBER_OF_LAUNCHES, numberOfLaunches);
                    qDebug() << numberOfLaunches << "launches of Xpiks with Telemetry OFF";
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
            path = QStandardPaths::locate(appDataLocationType, Constants::TERMS_AND_CONDITIONS_FILENAME);
#endif
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                text = file.readAll();
                file.close();
            } else {
                qWarning() << "terms_and_conditions.txt file is not found on path" << path;

                path = QDir::current().absoluteFilePath(QLatin1String(Constants::TERMS_AND_CONDITIONS_FILENAME));

                QFile currDirFile(path);
                if (currDirFile.open(QIODevice::ReadOnly)) {
                    text = currDirFile.readAll();
                    currDirFile.close();
                }
            }

            return text;
        }
    };
}

Q_DECLARE_METATYPE(Helpers::AppSettings*)

#endif // APPSETTINGS_H
