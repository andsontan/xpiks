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

#include "secretsmanager.h"
#include "aes-qt.h"

#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

namespace Encryption {
    SecretsManager::SecretsManager()
    {
        m_PasswordForMasterPassword = STRINGIZE(BUILDNUMBER);
        m_DefaultMasterPassword = "DefaultMasterPassword";
    }

    QByteArray SecretsManager::encodePassword(const QString &password) const {
        QString key = getKeyForEncryption();
        QByteArray encodedPassword = encodeText(password, key);
        return encodedPassword;
    }

    QString SecretsManager::decodePassword(const QByteArray &encodedPassword) const {
        QString key = getKeyForEncryption();
        QString decodedPassword = decodeText(encodedPassword, key);
        return decodedPassword;
    }

    QString SecretsManager::recodePassword(const QByteArray &encodedPassword, const QString &newMasterPassword) const
    {
        QString decodedPassword = decodePassword(encodedPassword);
        QString newEncodedPassword = encodeText(decodedPassword, newMasterPassword);
        return newEncodedPassword;
    }

    QString SecretsManager::getMasterPassword() const {
        QString masterPassword = decodeText(m_EncodedMasterPassword, m_PasswordForMasterPassword);
        return masterPassword;
    }

    void SecretsManager::setMasterPassword(const QString &masterPassword) {
        m_EncodedMasterPassword = encodeText(masterPassword, m_PasswordForMasterPassword);
    }

    QString SecretsManager::getKeyForEncryption() const {
        QString key;

        if (m_EncodedMasterPassword.isEmpty()) {
            key = m_DefaultMasterPassword;
        } else {
            key = decodeText(m_EncodedMasterPassword, m_PasswordForMasterPassword);
        }

        return key;
    }
}
