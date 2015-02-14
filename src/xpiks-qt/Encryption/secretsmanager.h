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

#ifndef SECRETSMANAGER_H
#define SECRETSMANAGER_H

#include <QByteArray>
#include <QObject>
#include <QString>

namespace Encryption {
    class SecretsManager : public QObject {
        Q_OBJECT

    public:
        SecretsManager();

    public:
        QByteArray encodePassword(const QString &password) const;
        QString decodePassword(const QByteArray &encodedPassword) const;
        // operation executed before setting new master password
        // old data gets reencoded with new master password
        QString recodePassword(const QByteArray &encodedPassword, const QString &newMasterPassword) const;

    public:
        QString getMasterPassword() const;
        void setMasterPassword(const QString &masterPassword);

    private:
        QString getKeyForEncryption() const;

    private:
        QString m_PasswordForMasterPassword;
        // not to keep in memory raw master password
        QByteArray m_EncodedMasterPassword;
        QString m_DefaultMasterPassword;
    };
}

#endif // SECRETSMANAGER_H
