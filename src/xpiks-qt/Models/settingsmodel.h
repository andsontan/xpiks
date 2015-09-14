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
    public:
        explicit SettingsModel(QObject *parent = 0);
        virtual ~SettingsModel() {}

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

    private:
        void resetToDefault();

    private:
        QString m_ExifToolPath;
        QString m_CurlPath;
        double m_MinMegapixelCount;
        int m_MaxDescriptionLength;
        int m_MaxKeywordsCount;
        int m_UploadTimeout; // in minutes
        bool m_MustUseMasterPassword;
        bool m_MustUseConfirmations;
        bool m_SaveBackups;
    };
}

#endif // SETTINGSMODEL_H
