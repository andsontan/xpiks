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

namespace Commands { class CommandManager; }

namespace Encryption {
    class SecretsManager : public QObject {
        Q_OBJECT

    public:
        SecretsManager();

        void setCommandManager(Commands::CommandManager *commandManager) {
            Q_ASSERT(commandManager != NULL);
            m_CommandManager = commandManager;
        }

    public:
        void setMasterPasswordHash(const QString &hash);

    public:
        QString encodePassword(const QString &password) const;
        QString decodePassword(const QString &encodedPassword) const;
        // operation executed before setting new master password
        // old data gets reencoded with new master password
        // could be static, but is instnance method for explicity
        QString recodePassword(const QString &encodedPassword,
                                  const QString &oldMasterPassword, const QString &newMasterPassword) const;

    public:
        Q_INVOKABLE QString getMasterPasswordHash() const { return QString::fromLatin1(m_MasterPasswordHash.toHex()); }
        Q_INVOKABLE bool testMasterPassword(const QString &masterPasswordCandidate) const;
        Q_INVOKABLE bool isMasterPasswordSet() const { return !m_MasterPasswordHash.isEmpty(); }

    private:
        QString getMasterPassword() const;

    public:
        Q_INVOKABLE void setMasterPassword(const QString &masterPassword);

    public:
        Q_INVOKABLE void resetMasterPassword();
        // should be executed on close of Upload dialog
        Q_INVOKABLE void purgeMasterPassword() { m_EncodedMasterPassword.clear(); }
        Q_INVOKABLE bool changeMasterPassword(bool firstTime, const QString &inputCurrMasterPassword,
                                              const QString &newMasterPassword);
        Q_INVOKABLE void removeMasterPassword() { m_EncodedMasterPassword.clear(); m_MasterPasswordHash.clear(); }

    signals:
        void beforeMasterPasswordChange(const QString &oldMasterPassword, const QString &newMasterPassword);

    private:
        QString getKeyForEncryption() const;
        QByteArray getPasswordHash(const QString &password) const;

    private:
        // for keeping in memory master password
        QString m_PasswordForMasterPassword;
        QString m_EncodedMasterPassword;
        QString m_DefaultMasterPassword;
        // used for checks in Upload dialog and changing MP
        QByteArray m_MasterPasswordHash;
        Commands::CommandManager *m_CommandManager;
    };
}

#endif // SECRETSMANAGER_H
