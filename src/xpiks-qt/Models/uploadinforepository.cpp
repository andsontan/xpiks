#include "uploadinforepository.h"

namespace Models {
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
            title = value.toString().trimmed();
            if (title.isEmpty()) title = "Untitled";
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
            uploadInfo->setPassword(value.toString().trimmed());
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
