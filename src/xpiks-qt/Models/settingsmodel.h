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

#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>

namespace Models {

    int ensureInBounds(int value, int boundA, int boundB);
    double ensureInBounds(double value, double boundA, double boundB);

    class SettingsModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString exifToolPath READ getExifToolPath WRITE setExifToolPath NOTIFY exifToolPathChanged)
        Q_PROPERTY(QString curlPath READ getCurlPath WRITE setCurlPath NOTIFY curlPathChanged)
        Q_PROPERTY(double minMegapixelCount READ getMinMegapixelCount WRITE setMinMegapixelCount NOTIFY minMegapixelCountChanged)
        Q_PROPERTY(int maxDescriptionLength READ getMaxDescriptionLength WRITE setMaxDescriptionLength NOTIFY maxDescriptionLengthChanged)
        Q_PROPERTY(int maxKeywordsCount READ getMaxKeywordsCount WRITE setMaxKeywordsCount NOTIFY maxKeywordsCountChanged)
        Q_PROPERTY(int uploadTimeout READ getUploadTimeout WRITE setUploadTimeout NOTIFY uploadTimeoutChanged)
        Q_PROPERTY(bool mustUseMasterPassword READ getMustUseMasterPassword WRITE setMustUseMasterPassword NOTIFY mustUseMasterPasswordChanged)
        Q_PROPERTY(bool mustUseConfirmations READ getMustUseConfirmations WRITE setMustUseConfirmations NOTIFY mustUseConfirmationsChanged)
        Q_PROPERTY(bool saveBackups READ getSaveBackups WRITE setSaveBackups NOTIFY saveBackupsChanged)
        Q_PROPERTY(double keywordSizeScale READ getKeywordSizeScale WRITE setKeywordSizeScale NOTIFY keywordSizeScaleChanged)
        Q_PROPERTY(int dismissDuration READ getDismissDuration WRITE setDismissDuration NOTIFY dismissDurationChanged)
        Q_PROPERTY(int maxParallelUploads READ getMaxParallelUploads WRITE setMaxParallelUploads NOTIFY maxParallelUploadsChanged)
        Q_PROPERTY(QString proxyURI READ getProxyURI WRITE setProxyURI NOTIFY proxyURIChanged)
        Q_PROPERTY(bool fitSmallPreview READ getFitSmallPreview WRITE setFitSmallPreview NOTIFY fitSmallPreviewChanged)
    public:
        explicit SettingsModel(QObject *parent = 0);
        virtual ~SettingsModel() {}

    public:

    public:
        Q_INVOKABLE void resetAllValues();
        Q_INVOKABLE void saveAllValues();
        Q_INVOKABLE void clearMasterPasswordSettings();
        Q_INVOKABLE void resetExifTool();
        Q_INVOKABLE void resetCurl();
        Q_INVOKABLE void readAllValues();
        Q_INVOKABLE void raiseMasterPasswordSignal() { emit mustUseMasterPasswordChanged(m_MustUseMasterPassword); }

    public:
        QString getExifToolPath() const { return m_ExifToolPath; }
        QString getCurlPath() const { return m_CurlPath; }
        double getMinMegapixelCount() const { return m_MinMegapixelCount; }
        int getMaxDescriptionLength() const { return m_MaxDescriptionLength; }
        int getMaxKeywordsCount() const { return m_MaxKeywordsCount; }
        int getUploadTimeout() const { return m_UploadTimeout; }
        bool getMustUseMasterPassword() const { return m_MustUseMasterPassword; }
        bool getMustUseConfirmations() const { return m_MustUseConfirmations; }
        bool getSaveBackups() const { return m_SaveBackups; }
        double getKeywordSizeScale() const { return m_KeywordSizeScale; }
        int getDismissDuration() const { return m_DismissDuration; }
        int getMaxParallelUploads() const { return m_MaxParallelUploads; }
        QString getProxyURI() const { return m_ProxyURI; }
        bool getFitSmallPreview() const { return m_FitSmallPreview; }

    signals:
        void exifToolPathChanged(QString exifToolPath);
        void curlPathChanged(QString curlPath);
        void minMegapixelCountChanged(double minMegapixelCount);
        void maxDescriptionLengthChanged(int maxDescriptionLength);
        void maxKeywordsCountChanged(int maxKeywordsCount);
        void uploadTimeoutChanged(int uploadTimeout);
        void mustUseMasterPasswordChanged(bool mustUseMasterPassword);
        void mustUseConfirmationsChanged(bool mustUseConfirmations);
        void saveBackupsChanged(bool saveBackups);
        void keywordSizeScaleChanged(double value);
        void dismissDurationChanged(int value);
        void maxParallelUploadsChanged(int value);
        void proxyURIChanged(QString value);
        void fitSmallPreviewChanged(bool value);

    public:
        void setExifToolPath(QString exifToolPath) {
            if (m_ExifToolPath == exifToolPath)
                return;

            m_ExifToolPath = exifToolPath;
            emit exifToolPathChanged(exifToolPath);
        }

        void setCurlPath(QString curlPath) {
            if (m_CurlPath == curlPath)
                return;

            m_CurlPath = curlPath;
            emit curlPathChanged(curlPath);
        }

        void setProxyURI(QString value) {
            if (m_ProxyURI == value)
                return;

            m_ProxyURI = value;
            emit proxyURIChanged(value);
        }

        void setMinMegapixelCount(double minMegapixelCount) {
            if (m_MinMegapixelCount == minMegapixelCount)
                return;

            m_MinMegapixelCount = ensureInBounds(minMegapixelCount, 0.0, 100.0);
            emit minMegapixelCountChanged(minMegapixelCount);
        }

        void setMaxDescriptionLength(int maxDescriptionLength) {
            if (m_MaxDescriptionLength == maxDescriptionLength)
                return;

            m_MaxDescriptionLength = ensureInBounds(maxDescriptionLength, 0, 500);
            emit maxDescriptionLengthChanged(maxDescriptionLength);
        }

        void setMaxKeywordsCount(int maxKeywordsCount) {
            if (m_MaxKeywordsCount == maxKeywordsCount)
                return;

            m_MaxKeywordsCount = ensureInBounds(maxKeywordsCount, 0, 1000);
            emit maxKeywordsCountChanged(maxKeywordsCount);
        }

        void setUploadTimeout(int uploadTimeout) {
            if (m_UploadTimeout == uploadTimeout)
                return;

            m_UploadTimeout = ensureInBounds(uploadTimeout, 0, 20);
            emit uploadTimeoutChanged(uploadTimeout);
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
            if (m_KeywordSizeScale == value)
                return;

            m_KeywordSizeScale = value;
            emit keywordSizeScaleChanged(value);
        }

        void setDismissDuration(int value) {
            if (m_DismissDuration == value)
                return;

            m_DismissDuration = ensureInBounds(value, 5, 20);
            emit dismissDurationChanged(value);
        }

        void setMaxParallelUploads(int value) {
            if (m_MaxParallelUploads == value)
                return;

            m_MaxParallelUploads = ensureInBounds(value, 1, 4);
            emit maxParallelUploadsChanged(value);
        }

        void setFitSmallPreview(bool value) {
            if (m_FitSmallPreview == value)
                return;

            m_FitSmallPreview = value;
            emit fitSmallPreviewChanged(value);
        }

    private:
        void resetToDefault();

    private:
        QString m_ExifToolPath;
        QString m_CurlPath;
        QString m_ProxyURI;
        double m_MinMegapixelCount;
        double m_KeywordSizeScale;
        int m_MaxDescriptionLength;
        int m_MaxKeywordsCount;
        int m_UploadTimeout; // in minutes
        int m_DismissDuration;
        int m_MaxParallelUploads;
        bool m_MustUseMasterPassword;
        bool m_MustUseConfirmations;
        bool m_SaveBackups;
        bool m_FitSmallPreview;
    };
}

#endif // SETTINGSMODEL_H
