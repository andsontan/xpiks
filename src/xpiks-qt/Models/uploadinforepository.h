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

#ifndef UPLOADINFOREPOSITORY_H
#define UPLOADINFOREPOSITORY_H

#include <QAbstractListModel>
#include <QDataStream>
#include <QByteArray>
#include <QHash>
#include <QList>
#include "uploadinfo.h"
#include "../Encryption/secretsmanager.h"

namespace Models {
    class UploadInfoRepository : public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(int infosCount READ getInfosCount NOTIFY infosCountChanged)
    public:
        UploadInfoRepository(QObject *parent = 0) :
            QAbstractListModel(parent)
        {}

        ~UploadInfoRepository() { qDeleteAll(m_UploadInfos); m_UploadInfos.clear();  }

        void setSecretsManager(Encryption::SecretsManager *secretsManager) { Q_ASSERT(secretsManager != NULL); m_SecretsManager = secretsManager; }
        void initFromString(const QString &savedString);

    public:
        enum UploadInfoRepositoryRoles {
            TitleRole = Qt::UserRole + 1,
            EditTitleRole,
            HostRole,
            EditHostRole,
            UsernameRole,
            EditUsernameRole,
            PasswordRole,
            EditPasswordRole,
            IsSelectedRole,
            EditIsSelectedRole
        };

        int getInfosCount() const { return m_UploadInfos.length(); }

    signals:
        void infosCountChanged();

    public:
        Q_INVOKABLE void removeItem(int row) {
            beginRemoveRows(QModelIndex(), row, row);
            removeInnerItem(row);
            endRemoveRows();
            emit infosCountChanged();
        }

        Q_INVOKABLE void addItem() {
            int lastIndex = m_UploadInfos.length();
            beginInsertRows(QModelIndex(), lastIndex, lastIndex);
            m_UploadInfos.append(new UploadInfo());
            endInsertRows();
            emit infosCountChanged();
        }

        Q_INVOKABLE QString getInfoString() const {
            QList<QHash<int, QString> > items;
            foreach (UploadInfo *info, m_UploadInfos) {
                items.append(info->toHash());
            }

            // TODO: move to SFTP
            // while stocks use FTP, sophisticated passwords
            // saving on client side is useless
            QByteArray result;
            QDataStream stream( &result, QIODevice::WriteOnly);
            stream << items;
            return result.toBase64();
        }

        Q_INVOKABLE int getSelectedInfosCount() const {
            int count = 0;
            foreach (UploadInfo *info, m_UploadInfos) {
                if (info->getIsSelected()) count++;
            }
            return count;
        }

        Q_INVOKABLE QString getAgenciesWithMissingDetails();

    public:
        const QList<UploadInfo*> &getUploadInfos() const { return m_UploadInfos; }

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        void onBeforeMasterPasswordChanged(const QString &oldMasterPassword, const QString &newMasterPassword) {
            Q_ASSERT(m_SecretsManager != NULL);
            foreach (UploadInfo *info, m_UploadInfos) {
                if (info->hasPassword()) {
                    QByteArray newPassword = m_SecretsManager->recodePassword(
                                info->getPassword(), oldMasterPassword, newMasterPassword);
                    info->setPassword(newPassword);
                }
            }
        }

    protected:
        QHash<int, QByteArray> roleNames() const;

        void removeInnerItem(int row) {
            UploadInfo *info = m_UploadInfos.takeAt(row);
            delete info;
        }

    private:
        QList<UploadInfo*> m_UploadInfos;
        Encryption::SecretsManager *m_SecretsManager;
    };
}

#endif // UPLOADINFOREPOSITORY_H
