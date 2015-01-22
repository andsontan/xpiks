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

#include "uploadinforepository.h"

namespace Models {
    void UploadInfoRepository::initFromString(const QString &savedString)
    {
        QStringList items = savedString.split('|', QString::SkipEmptyParts);
        foreach (const QString &item, items) {
            UploadInfo *info = new UploadInfo(item);
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

        return items.join(',');
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
        case PasswordRole:
            return uploadInfo->getPassword();
        case IsSelectedRole:
            return uploadInfo->getIsSelected();
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
        switch (role) {
        case EditTitleRole:
            roleToUpdate = TitleRole;
            title = value.toString();
            uploadInfo->setTitle(title);
            break;
        case EditHostRole:
            roleToUpdate = HostRole;
            uploadInfo->setHost(value.toString().trimmed());
            break;
        case EditUsernameRole:
            roleToUpdate = UsernameRole;
            uploadInfo->setUsername(value.toString().trimmed());
            break;
        case EditPasswordRole:
            roleToUpdate = PasswordRole;
            uploadInfo->setPassword(value.toString());
            break;
        case EditIsSelectedRole:
            roleToUpdate = IsSelectedRole;
            uploadInfo->setIsSelected(value.toBool());
        default:
            return false;
        }

        emit dataChanged(index, index, QVector<int>() << role << roleToUpdate);
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
        return roles;
    }
}
