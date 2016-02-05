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

#include "settingsmodel.h"
#include <QQmlEngine>
#include <QDebug>
#include "../Helpers/appsettings.h"
#include "../Common/defines.h"
#include "../Commands/commandmanager.h"

#ifdef Q_OS_MAC
#define DEFAULT_EXIFTOOL "/usr/bin/exiftool"
#else
#define DEFAULT_EXIFTOOL "exiftool"
#endif

#define DEFAULT_DICT_PATH ""
#define DEFAULT_MAX_KEYWORDS 50
#define DEFAULT_MAX_DESCRIPTION 200
#define DEFAULT_MIN_MEGAPIXELS 4.0
#define DEFAULT_USE_MASTERPASSWORD false
#define DEFAULT_UPLOAD_TIMEOUT 10
#define DEFAULT_USE_CONFIRMATIONS true
#define DEFAULT_SAVE_BACKUPS true
#define DEFAULT_KEYWORD_SIZE_SCALE 1.0
#define DEFAULT_DISMISS_DURATION 10
#define DEFAULT_MAX_PARALLEL_UPLOADS 2
#define DEFAULT_FIT_SMALL_PREVIEW false
#define DEFAULT_SEARCH_USING_AND true
#define DEFAULT_SCROLL_SPEED_SCALE 1.0
#define DEFAULT_USE_SPELL_CHECK true
#define DEFAULT_HAVE_USER_CONSENT false
#define DEFAULT_COLLECT_USER_STATISTIC true


namespace Models {
    SettingsModel::SettingsModel(QObject *parent) :
        QObject(parent),
        m_ExifToolPath(DEFAULT_EXIFTOOL),
        m_DictPath(DEFAULT_DICT_PATH),
        m_MinMegapixelCount(DEFAULT_MIN_MEGAPIXELS),
        m_KeywordSizeScale(DEFAULT_KEYWORD_SIZE_SCALE),
        m_ScrollSpeedScale(DEFAULT_SCROLL_SPEED_SCALE),
        m_MaxDescriptionLength(DEFAULT_MAX_DESCRIPTION),
        m_MaxKeywordsCount(DEFAULT_MAX_KEYWORDS),
        m_UploadTimeout(DEFAULT_UPLOAD_TIMEOUT),
        m_DismissDuration(DEFAULT_DISMISS_DURATION),
        m_MaxParallelUploads(DEFAULT_MAX_PARALLEL_UPLOADS),
        m_MustUseMasterPassword(DEFAULT_USE_MASTERPASSWORD),
        m_MustUseConfirmations(DEFAULT_USE_CONFIRMATIONS),
        m_SaveBackups(DEFAULT_SAVE_BACKUPS),
        m_FitSmallPreview(DEFAULT_FIT_SMALL_PREVIEW),
        m_SearchUsingAnd(DEFAULT_SEARCH_USING_AND),
        m_UseSpellCheck(DEFAULT_USE_SPELL_CHECK),
        m_UserStatistic(DEFAULT_COLLECT_USER_STATISTIC),
        m_DictsPathChanged(false)
    {
    }

    void SettingsModel::resetAllValues() {
        resetToDefault();
        saveAllValues();
    }

    void SettingsModel::saveAllValues() {
        qInfo() << "SettingsModel::saveAllValues #";

        Helpers::AppSettings appSettings;
        appSettings.setValue(appSettings.getExifToolPathKey(), m_ExifToolPath);
        appSettings.setValue(appSettings.getDictionaryPathKey(), m_DictPath);
        appSettings.setValue(appSettings.getMinMegapixelCountKey(), m_MinMegapixelCount);
        appSettings.setValue(appSettings.getMaxDescriptionLengthKey(), m_MaxDescriptionLength);
        appSettings.setValue(appSettings.getMaxKeywordsCountKey(), m_MaxKeywordsCount);
        appSettings.setValue(appSettings.getOneUploadSecondsTimeoutKey(), m_UploadTimeout);
        appSettings.setValue(appSettings.getMustUseMasterPasswordKey(), m_MustUseMasterPassword);
        appSettings.setValue(appSettings.getUseConfirmationDialogsKey(), m_MustUseConfirmations);
        appSettings.setValue(appSettings.getSaveBackupsKey(), m_SaveBackups);
        appSettings.setValue(appSettings.getKeywordSizeScaleKey(), m_KeywordSizeScale);
        appSettings.setValue(appSettings.getDismissDurationKey(), m_DismissDuration);
        appSettings.setValue(appSettings.getMaxParallelUploadsKey(), m_MaxParallelUploads);
        appSettings.setValue(appSettings.getFitSmallPreviewKey(), m_FitSmallPreview);
        appSettings.setValue(appSettings.getSearchUsingAndKey(), m_SearchUsingAnd);
        appSettings.setValue(appSettings.getScrollSpeedScaleKey(), m_ScrollSpeedScale);
        appSettings.setValue(appSettings.getUseSpellCheckKey(), m_UseSpellCheck);
        appSettings.setValue(appSettings.getUserStatisticKey(), m_UserStatistic);

        if (!m_MustUseMasterPassword) {
            appSettings.setValue(appSettings.getMasterPasswordHashKey(), "");
        }

        emit keywordSizeScaleChanged(m_KeywordSizeScale);

#if defined(Q_OS_LINUX)
        if (m_DictsPathChanged) {
            // TODO: check if need to restart depending on path
            m_CommandManager->restartSpellChecking();
            m_DictsPathChanged = false;
        }
#endif
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

    void SettingsModel::resetDictPath() {
        setDictionaryPath(DEFAULT_DICT_PATH);
    }

    void SettingsModel::readAllValues() {
        qInfo() << "SettingsModel::readAllValues #";

        Helpers::AppSettings appSettings;
        setExifToolPath(appSettings.value(appSettings.getExifToolPathKey(), DEFAULT_EXIFTOOL).toString());
        setDictionaryPath(appSettings.value(appSettings.getDictionaryPathKey(), DEFAULT_DICT_PATH).toString());
        setMinMegapixelCount(appSettings.doubleValue(appSettings.getMinMegapixelCountKey(), DEFAULT_MIN_MEGAPIXELS));
        setMaxDescriptionLength(appSettings.value(appSettings.getMaxDescriptionLengthKey(), DEFAULT_MAX_DESCRIPTION).toInt());
        setMaxKeywordsCount(appSettings.value(appSettings.getMaxKeywordsCountKey(), DEFAULT_MAX_KEYWORDS).toInt());
        setUploadTimeout(appSettings.value(appSettings.getOneUploadSecondsTimeoutKey(), DEFAULT_UPLOAD_TIMEOUT).toInt());
        setMustUseMasterPassword(appSettings.boolValue(appSettings.getMustUseMasterPasswordKey(), DEFAULT_USE_MASTERPASSWORD));
        setMustUseConfirmations(appSettings.boolValue(appSettings.getUseConfirmationDialogsKey(), DEFAULT_USE_CONFIRMATIONS));
        setSaveBackups(appSettings.boolValue(appSettings.getSaveBackupsKey(), DEFAULT_SAVE_BACKUPS));
        setKeywordSizeScale(appSettings.doubleValue(appSettings.getKeywordSizeScaleKey(), DEFAULT_KEYWORD_SIZE_SCALE));
        setDismissDuration(appSettings.value(appSettings.getDismissDurationKey(), DEFAULT_DISMISS_DURATION).toInt());
        setMaxParallelUploads(appSettings.value(appSettings.getMaxParallelUploadsKey(), DEFAULT_MAX_PARALLEL_UPLOADS).toInt());
        setFitSmallPreview(appSettings.boolValue(appSettings.getFitSmallPreviewKey(), DEFAULT_FIT_SMALL_PREVIEW));
        setSearchUsingAnd(appSettings.boolValue(appSettings.getSearchUsingAndKey(), DEFAULT_SEARCH_USING_AND));
        setScrollSpeedScale(appSettings.doubleValue(appSettings.getScrollSpeedScaleKey(), DEFAULT_SCROLL_SPEED_SCALE));
        setUseSpellCheck(appSettings.boolValue(appSettings.getUseSpellCheckKey(), DEFAULT_USE_SPELL_CHECK));
        setUserStatistic(appSettings.boolValue(appSettings.getUserStatisticKey(), DEFAULT_COLLECT_USER_STATISTIC));
    }

    void SettingsModel::resetToDefault() {
        qInfo() << "SettingsModel::resetToDefault #";

        setExifToolPath(DEFAULT_EXIFTOOL);
        setDictionaryPath(DEFAULT_DICT_PATH);
        setMinMegapixelCount(DEFAULT_MIN_MEGAPIXELS);
        setMaxDescriptionLength(DEFAULT_MAX_DESCRIPTION);
        setMaxKeywordsCount(DEFAULT_MAX_KEYWORDS);
        setUploadTimeout(DEFAULT_UPLOAD_TIMEOUT);
        setMustUseMasterPassword(DEFAULT_USE_MASTERPASSWORD);
        setMustUseConfirmations(DEFAULT_USE_CONFIRMATIONS);
        setSaveBackups(DEFAULT_SAVE_BACKUPS);
        setKeywordSizeScale(DEFAULT_KEYWORD_SIZE_SCALE);
        setDismissDuration(DEFAULT_DISMISS_DURATION);
        setMaxParallelUploads(DEFAULT_MAX_PARALLEL_UPLOADS);
        setFitSmallPreview(DEFAULT_FIT_SMALL_PREVIEW);
        setSearchUsingAnd(DEFAULT_SEARCH_USING_AND);
        setScrollSpeedScale(DEFAULT_SCROLL_SPEED_SCALE);
        setUseSpellCheck(DEFAULT_USE_SPELL_CHECK);
        setUserStatistic(DEFAULT_COLLECT_USER_STATISTIC);

#if defined(QT_DEBUG)
        Helpers::AppSettings appSettings;
        appSettings.setValue(appSettings.getUserConsentKey(), DEFAULT_HAVE_USER_CONSENT);
        appSettings.setValue(appSettings.getInstalledVersionKey(), 0);
#endif
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

