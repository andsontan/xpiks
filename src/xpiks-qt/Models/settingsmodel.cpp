/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Helpers/appsettings.h"

#ifdef Q_OS_MAC
#define DEFAULT_EXIFTOOL "/usr/local/bin/exiftool"
#else
#define DEFAULT_EXIFTOOL "exiftool"
#endif

#define DEFAULT_CURL "curl"
#define DEFAULT_MAX_KEYWORDS 50
#define DEFAULT_MAX_DESCRIPTION 200
#define DEFAULT_MIN_MEGAPIXELS 4.0
#define DEFAULT_USE_MASTERPASSWORD false
#define DEFAULT_TIMEOUT 10
#define DEFAULT_USE_CONFIRMATIONS true
#define DEFAULT_SAVE_BACKUPS true

namespace Models {
    SettingsModel::SettingsModel(QObject *parent) :
        QObject(parent)
    {
        readAllValues();
    }

    void SettingsModel::resetAllValues() {
        resetToDefault();
        saveAllValues();
    }

    void SettingsModel::saveAllValues() {
        Helpers::AppSettings appSettings;
        appSettings.setValue(appSettings.getExifToolPathKey(), m_ExifToolPath);
        appSettings.setValue(appSettings.getCurlPathKey(), m_CurlPath);
        appSettings.setValue(appSettings.getMinMegapixelCountKey(), m_MinMegapixelCount);
        appSettings.setValue(appSettings.getMaxDescriptionLengthKey(), m_MaxDescriptionLength);
        appSettings.setValue(appSettings.getMaxKeywordsCountKey(), m_MaxKeywordsCount);
        appSettings.setValue(appSettings.getOneUploadMinutesTimeoutKey(), m_UploadTimeout);
        appSettings.setValue(appSettings.getMustUseMasterPasswordKey(), m_MustUseMasterPassword);
        appSettings.setValue(appSettings.getUseConfirmationDialogsKey(), m_MustUseConfirmations);
        appSettings.setValue(appSettings.getSaveBackupsKey(), m_SaveBackups);

        if (!m_MustUseMasterPassword) {
            appSettings.setValue(appSettings.getMasterPasswordHashKey(), "");
        }
    }

    void SettingsModel::clearMasterPasswordSettings() {
        setMustUseMasterPassword(false);
        Helpers::AppSettings appSettings;
        appSettings.setValue(appSettings.getMasterPasswordHashKey(), "");
        appSettings.setValue(appSettings.getMustUseMasterPasswordKey(), false);
    }

    void SettingsModel::resetExifTool() {
        setExifToolPath(DEFAULT_EXIFTOOL);
    }

    void SettingsModel::resetCurl() {
        setCurlPath(DEFAULT_CURL);
    }

    void SettingsModel::readAllValues() {
        Helpers::AppSettings appSettings;
        setExifToolPath(appSettings.value(appSettings.getExifToolPathKey(), DEFAULT_EXIFTOOL).toString());
        setCurlPath(appSettings.value(appSettings.getCurlPathKey(), DEFAULT_CURL).toString());
        setMinMegapixelCount(appSettings.value(appSettings.getMinMegapixelCountKey(), DEFAULT_MIN_MEGAPIXELS).toDouble());
        setMaxDescriptionLength(appSettings.value(appSettings.getMaxDescriptionLengthKey(), DEFAULT_MAX_DESCRIPTION).toInt());
        setMaxKeywordsCount(appSettings.value(appSettings.getMaxKeywordsCountKey(), DEFAULT_MAX_KEYWORDS).toInt());
        setUploadTimeout(appSettings.value(appSettings.getOneUploadMinutesTimeoutKey(), DEFAULT_TIMEOUT).toInt());
        setMustUseMasterPassword(appSettings.boolValue(appSettings.getMustUseMasterPasswordKey(), DEFAULT_USE_MASTERPASSWORD));
        setMustUseConfirmations(appSettings.boolValue(appSettings.getUseConfirmationDialogsKey(), DEFAULT_USE_CONFIRMATIONS));
        setSaveBackups(appSettings.boolValue(appSettings.getSaveBackupsKey(), DEFAULT_SAVE_BACKUPS));
    }

    void SettingsModel::resetToDefault() {
        setExifToolPath(DEFAULT_EXIFTOOL);
        setCurlPath(DEFAULT_CURL);
        setMinMegapixelCount(DEFAULT_MIN_MEGAPIXELS);
        setMaxDescriptionLength(DEFAULT_MAX_DESCRIPTION);
        setMaxKeywordsCount(DEFAULT_MAX_KEYWORDS);
        setUploadTimeout(DEFAULT_TIMEOUT);
        setMustUseMasterPassword(DEFAULT_USE_MASTERPASSWORD);
        setMustUseConfirmations(DEFAULT_USE_CONFIRMATIONS);
        setSaveBackups(DEFAULT_SAVE_BACKUPS);
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

}

