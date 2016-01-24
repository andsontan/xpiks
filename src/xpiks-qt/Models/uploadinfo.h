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

#ifndef UPLOADINFO
#define UPLOADINFO

#include <QHash>
#include <QMutex>
#include <QString>
#include <QByteArray>

namespace Models {
    class UploadInfo {
    private:
        enum UField { TitleField = 0, HostField, UsernameField, PasswordField, DirectoryField, ZipField, FtpPassiveModeField };

    public:
        UploadInfo():
            m_Percent(0),
            m_ZipBeforeUpload(false),
            m_IsSelected(false),
            m_FtpPassiveMode(false)
        {
            m_Title = QString::fromLatin1("Untitled");
        }

        UploadInfo(const QHash<int, QString>& items) :
            m_Percent(0),
            m_IsSelected(false)
        {
            QString emptyString = QString::fromLatin1("");
            m_Title = items.value(TitleField, QString::fromLatin1("Untitled"));
            m_Host = items.value(HostField, emptyString);
            m_Username = items.value(UsernameField, emptyString);
            m_EncodedPassword = items.value(PasswordField, emptyString);
            m_ZipBeforeUpload = items.value(ZipField, "false") == QLatin1String("true");
            m_FtpPassiveMode = items.value(FtpPassiveModeField, "false") == QLatin1String("true");
        }

    public:
        const QString &getTitle() const { return m_Title; }
        const QString &getHost() const { return m_Host; }
        const QString &getUsername() const { return m_Username; }
        QString getPassword() { QString result; m_Mutex.lock(); result = m_EncodedPassword; m_Mutex.unlock(); return result; }
        bool hasPassword() const { return !m_EncodedPassword.isEmpty(); }
        bool getIsSelected() const { return m_IsSelected; }
        bool getZipBeforeUpload() const { return m_ZipBeforeUpload; }
        bool isSomethingMissing() const { return m_EncodedPassword.isEmpty() || m_Host.isEmpty() || m_Username.isEmpty(); }
        bool isEmpty() const { return m_EncodedPassword.isEmpty() && m_Host.isEmpty() && m_Username.isEmpty() &&
                    (m_Title.isEmpty() || m_Title == QLatin1String("Untitled")); }
        int getPercent() const { return m_Percent; }
        bool getFtpPassiveMode() const { return m_FtpPassiveMode; }

    public:
        bool setTitle(const QString &value) { bool result = m_Title != value; m_Title = value; return result; }
        bool setHost(const QString &value) { bool result = m_Host != value; m_Host = value; return result; }
        bool setUsername(const QString &value) { bool result = m_Username != value; m_Username = value; return result; }
        bool setPassword(const QString &value) {
            bool result = value != m_EncodedPassword;
            if (result) {
                m_Mutex.lock();
                {
                    result = value != m_EncodedPassword;
                    if (result) {
                        m_EncodedPassword = value;
                    }
                }
                m_Mutex.unlock();
            }
            return result;
        }
        bool setIsSelected(bool value) { bool result = m_IsSelected != value; m_IsSelected = value; return result; }
        bool setZipBeforeUpload(bool value) { bool result = m_ZipBeforeUpload != value; m_ZipBeforeUpload = value; return result; }
        void restorePassword() { m_EncodedPassword = m_EncodedPasswordBackup; }
        void backupPassword() { m_EncodedPasswordBackup = m_EncodedPassword; }
        void dropPassword() { m_EncodedPassword = ""; }
        void setPercent(int value) { m_Percent = value; }
        void resetPercent() { m_Percent = 0; }
        bool setFtpPassiveMode(bool value) { bool result = m_FtpPassiveMode != value; m_FtpPassiveMode = value; return result; }

    public:
        QHash<int, QString> toHash() {
            QHash<int, QString> hash;
            hash[TitleField] = m_Title.simplified().isEmpty() ? QString::fromLatin1("Untitled") : m_Title;
            hash[HostField] = m_Host;
            hash[UsernameField] = m_Username;
            hash[PasswordField] = m_EncodedPassword;
            hash[ZipField] = m_ZipBeforeUpload ? "true" : "false";
            hash[FtpPassiveModeField] = m_FtpPassiveMode ? "true" : "false";
            return hash;
        }

    private:
        QMutex m_Mutex;
        QString m_Title;
        QString m_Host;
        QString m_Username;
        QString m_EncodedPassword;
        // used for backup when MP is incorrect
        QString m_EncodedPasswordBackup;
        volatile int m_Percent;
        bool m_ZipBeforeUpload;
        bool m_IsSelected;
        bool m_FtpPassiveMode;
    };
}

#endif // UPLOADINFO

