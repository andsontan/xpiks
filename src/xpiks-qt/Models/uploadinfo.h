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
            m_Title = "Untitled";
        }

        UploadInfo(const QHash<int, QString>& items) :
            m_IsSelected(false)
        {
            m_Title = items.value(TitleField, "Untitled");
            m_Host = items.value(HostField, "");
            m_Username = items.value(UsernameField, "");

            QByteArray base64;
            QString serialized = items.value(PasswordField, "");
            base64.append(serialized);
            // TODO: replace this when Qt will have a nice way for it
            m_EncodedPassword = base64.fromBase64(base64);
        }

    public:
        const QString &getTitle() const { return m_Title; }
        const QString &getHost() const { return m_Host; }
        const QString &getUsername() const { return m_Username; }
        const QByteArray &getPassword() const { return m_EncodedPassword; }
        bool hasPassword() const { return m_EncodedPassword.isEmpty(); }
        bool getIsSelected() const { return m_IsSelected; }
        bool isSomethingMissing() const { return m_EncodedPassword.isEmpty() || m_Host.isEmpty() || m_Username.isEmpty(); }

    public:
        void setTitle(const QString &value) { m_Title = value; }
        void setHost(const QString &value) { m_Host = value; }
        void setUsername(const QString &value) { m_Username = value; }
        void setPassword(const QByteArray &value) { m_EncodedPassword = value; }
        void setIsSelected(bool value) { m_IsSelected = value; }

    public:
        QHash<int, QString> toHash() {
            QHash<int, QString> hash;
            hash[TitleField] = m_Title.trimmed().isEmpty() ? "Untitled" : m_Title;
            hash[HostField] = m_Host;
            hash[UsernameField] = m_Username;
            hash[PasswordField] = QString(m_EncodedPassword.toBase64());
            return hash;
        }

    private:
        QString m_Title;
        QString m_Host;
        QString m_Username;
        QByteArray m_EncodedPassword;
        bool m_IsSelected;
    };
}

#endif // UPLOADINFO

