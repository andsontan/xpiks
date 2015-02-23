/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Encryption/secretsmanager.h"

namespace Models {
    class UploadInfo {
    private:
        enum UField { TitleField = 0, HostField, UsernameField, PasswordField };

    public:
        UploadInfo():
            m_IsSelected(false)
        {
            m_Title = QString::fromLatin1("Untitled");
        }

        UploadInfo(const QHash<int, QString>& items) :
            m_IsSelected(false)
        {
            QString emptyString = QString::fromLatin1("");
            m_Title = items.value(TitleField, QString::fromLatin1("Untitled"));
            m_Host = items.value(HostField, emptyString);
            m_Username = items.value(UsernameField, emptyString);
            m_EncodedPassword = items.value(PasswordField, emptyString);
        }

    public:
        const QString &getTitle() const { return m_Title; }
        const QString &getHost() const { return m_Host; }
        const QString &getUsername() const { return m_Username; }
        QString getPassword() { QString result; m_Mutex.lock(); result = m_EncodedPassword; m_Mutex.unlock(); return result; }
        bool hasPassword() const { return !m_EncodedPassword.isEmpty(); }
        bool getIsSelected() const { return m_IsSelected; }
        bool isSomethingMissing() const { return m_EncodedPassword.isEmpty() || m_Host.isEmpty() || m_Username.isEmpty(); }
        bool isEmpty() const { return m_EncodedPassword.isEmpty() && m_Host.isEmpty() && m_Username.isEmpty() &&
                    (m_Title.isEmpty() || m_Title == QString::fromLatin1("Untitled")); }

    public:
        void setTitle(const QString &value) { m_Title = value; }
        void setHost(const QString &value) { m_Host = value; }
        void setUsername(const QString &value) { m_Username = value; }
        void setPassword(const QString &value) {
            m_Mutex.lock();
            {
                m_EncodedPassword = value;
            }
            m_Mutex.unlock();
        }
        void setIsSelected(bool value) { m_IsSelected = value; }
        void restorePassword() { m_EncodedPassword = m_EncodedPasswordBackup; }
        void backupPassword() { m_EncodedPasswordBackup = m_EncodedPassword; }
        void dropPassword() { m_EncodedPassword = ""; }

    public:
        QHash<int, QString> toHash() {
            QHash<int, QString> hash;
            hash[TitleField] = m_Title.trimmed().isEmpty() ? QString::fromLatin1("Untitled") : m_Title;
            hash[HostField] = m_Host;
            hash[UsernameField] = m_Username;
            hash[PasswordField] = m_EncodedPassword;

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
        bool m_IsSelected;
    };
}

#endif // UPLOADINFO

