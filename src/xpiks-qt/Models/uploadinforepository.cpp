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

#include <QDebug>
#include "uploadinforepository.h"

namespace Models {
    void UploadInfoRepository::initFromString(const QString &savedString)
    {
        QByteArray originalData;
        originalData.append(savedString.toLatin1());
        QByteArray result = QByteArray::fromBase64(originalData);

        QList<QHash<int, QString> > items;
        QDataStream stream(&result, QIODevice::ReadOnly);
        stream >> items;

        int length = items.length();
        for (int i = 0; i < length; ++i) {
            QHash<int, QString> hash = items[i];
            UploadInfo *info = new UploadInfo(hash);
            m_UploadInfos.append(info);
        }
    }

    QString UploadInfoRepository::getAgenciesWithMissingDetails()
    {
        QStringList items;
        foreach (UploadInfo *info, m_UploadInfos) {
            if (info->getIsSelected() && info->isSomethingMissing()) {
                items.append(info->getTitle());
            }
        }

        return items.join(", ");
    }

    // mp == master password
    void UploadInfoRepository::initializeAccounts(bool mpIsCorrectOrEmpty)
    {
        this->setEmptyPasswordsMode(!mpIsCorrectOrEmpty);
        if (!mpIsCorrectOrEmpty) {
            this->backupAndDropRealPasswords();
        }
    }

    void UploadInfoRepository::finalizeAccounts()
    {
        if (m_EmptyPasswordsMode) {
            this->restoreRealPasswords();
        }
    }

    int UploadInfoRepository::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_UploadInfos.count();
    }

    QVariant UploadInfoRepository::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_UploadInfos.count())
            return QVariant();

        UploadInfo *uploadInfo = m_UploadInfos[index.row()];

        switch (role) {
        case TitleRole:
            return uploadInfo->getTitle();
        case HostRole:
            return uploadInfo->getHost();
        case UsernameRole:
            return uploadInfo->getUsername();
        case PasswordRole: {
            const QString &encodedPassword = uploadInfo->getPassword();
            QString password = m_CommandManager->getSecretsManager()->decodePassword(encodedPassword);
            return password;
        }
        case IsSelectedRole:
            return uploadInfo->getIsSelected();
        case ZipBeforeUploadRole:
            return uploadInfo->getZipBeforeUpload();
        case UploadDirectoryRole:
            return uploadInfo->getUploadDirectory();
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags UploadInfoRepository::flags(const QModelIndex &index) const
    {
        if (!index.isValid()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool UploadInfoRepository::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid()) {
            return false;
        }

        UploadInfo *uploadInfo = m_UploadInfos[index.row()];
        QString title;
        int roleToUpdate = 0;
        bool needToUpdate = false;
        switch (role) {
        case EditTitleRole:
            roleToUpdate = TitleRole;
            title = value.toString();
            needToUpdate = uploadInfo->setTitle(title);
            break;
        case EditHostRole:
            roleToUpdate = HostRole;
            needToUpdate = uploadInfo->setHost(value.toString().trimmed());
            break;
        case EditUsernameRole:
            roleToUpdate = UsernameRole;
            needToUpdate = uploadInfo->setUsername(value.toString().trimmed());
            break;
        case EditPasswordRole: {
            roleToUpdate = PasswordRole;
            QString rawPassword = value.toString();
            QString encodedPassword = m_CommandManager->getSecretsManager()->encodePassword(rawPassword);
            // skip needUpdate
            uploadInfo->setPassword(encodedPassword);
            break;
        }
        case EditIsSelectedRole:
            roleToUpdate = IsSelectedRole;
            needToUpdate = uploadInfo->setIsSelected(value.toBool());
            break;
        case EditZipBeforeUploadRole:
            roleToUpdate = ZipBeforeUploadRole;
            needToUpdate = uploadInfo->setZipBeforeUpload(value.toBool());
            break;
        case EditUploadDirectoryRole:
            roleToUpdate = UploadDirectoryRole;
            needToUpdate = uploadInfo->setUploadDirectory(value.toString().trimmed());
            break;
        default:
            return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << roleToUpdate);
        }

        return true;
    }

    QHash<int, QByteArray> UploadInfoRepository::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[HostRole] = "host";
        roles[UsernameRole] = "username";
        roles[PasswordRole] = "password";
        roles[EditTitleRole] = "edittitle";
        roles[EditHostRole] = "edithost";
        roles[EditUsernameRole] = "editusername";
        roles[EditPasswordRole] = "editpassword";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[ZipBeforeUploadRole] = "zipbeforeupload";
        roles[EditZipBeforeUploadRole] = "editzipbeforeupload";
        roles[UploadDirectoryRole] = "uploaddirectory";
        roles[EditUploadDirectoryRole] = "edituploaddirectory";
        return roles;
    }
}
