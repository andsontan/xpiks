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

#include "uploadinforepository.h"
#include "uploadinfo.h"
#include "../Commands/commandmanager.h"
#include "../Encryption/secretsmanager.h"
#include "../Common/defines.h"

namespace Models {
    UploadInfoRepository::~UploadInfoRepository() { m_UploadInfos.clear();  }

    void UploadInfoRepository::initFromString(const QString &savedString) {
        LOG_DEBUG << "#";
        QByteArray originalData;
        originalData.append(savedString.toLatin1());
        QByteArray result = QByteArray::fromBase64(originalData);

        // bad type QList instead of QVector
        // but users already have this
        QList<QHash<int, QString> > items;
        QDataStream stream(&result, QIODevice::ReadOnly);
        stream >> items;

        int length = items.length();
        m_UploadInfos.reserve(length);

        for (int i = 0; i < length; ++i) {
            auto &hash = items.at(i);
            m_UploadInfos.emplace_back(new UploadInfo(hash));
        }

        LOG_INFO << length << "item(s) found";
    }

    void UploadInfoRepository::addItem() {
        int lastIndex = (int)m_UploadInfos.size();

        LOG_INFO << lastIndex;
        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        m_UploadInfos.emplace_back(new UploadInfo());
        endInsertRows();
        emit infosCountChanged();
    }

    QString UploadInfoRepository::getInfoString() const {
        LOG_INFO << "Serializing" << m_UploadInfos.size() << "info(s)";
        // bad type QList instead of QVector
        // but users already have this
        QList<QHash<int, QString> > items;
        for (auto &info: m_UploadInfos) {
            if (!info->isEmpty()) {
                items.append(info->toHash());
            }
        }

        // TODO: move to SFTP
        // while stocks use FTP, sophisticated passwords
        // saving on client side is useless
        QByteArray result;
        QDataStream stream(&result, QIODevice::WriteOnly);
        stream << items;
        return QString::fromUtf8(result.toBase64());
    }

    int UploadInfoRepository::getSelectedInfosCount() const {
        int selectedCount = 0;

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    QString UploadInfoRepository::getAgenciesWithMissingDetails() {
        QStringList items;

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected() && info->isSomethingMissing()) {
                items.append(info->getTitle());
            }
        }

        return items.join(", ");
    }

    void UploadInfoRepository::updateProperties(int itemIndex) {
        QModelIndex itemModelIndex = index(itemIndex);
        // could be any role - just to triger Advanced tab checkbox connections
        emit dataChanged(itemModelIndex, itemModelIndex, QVector<int>() << ZipBeforeUploadRole);
    }

    // mp == master password
    void UploadInfoRepository::initializeAccounts(bool mpIsCorrectOrEmpty) {
        this->setEmptyPasswordsMode(!mpIsCorrectOrEmpty);
        if (!mpIsCorrectOrEmpty) {
            this->backupAndDropRealPasswords();
        }
    }

    void UploadInfoRepository::finalizeAccounts() {
        if (m_EmptyPasswordsMode) {
            this->restoreRealPasswords();
        }
    }

    void UploadInfoRepository::backupAndDropRealPasswords() {
        for (auto &info: m_UploadInfos) {
            info->backupPassword();
            info->dropPassword();
        }
    }

    void UploadInfoRepository::restoreRealPasswords() {
        for (auto &info: m_UploadInfos) {
            info->restorePassword();
        }
    }

    void UploadInfoRepository::updatePercentages() {
        emit dataChanged(index(0), index((int)m_UploadInfos.size() - 1), QVector<int>() << PercentRole);
    }

    void UploadInfoRepository::resetPercents() {
        LOG_DEBUG << "#";
        for (auto &info: m_UploadInfos) {
            info->resetPercent();
        }
    }

    std::vector<std::shared_ptr<UploadInfo> > UploadInfoRepository::retrieveSelectedUploadInfos() const {
        std::vector<std::shared_ptr<UploadInfo> > uploadInfos;
        uploadInfos.reserve(m_UploadInfos.size());

        for (auto &info: m_UploadInfos) {
            if (info->getIsSelected()) {
                uploadInfos.push_back(info);
            }
        }

        return uploadInfos;
    }

    int UploadInfoRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_UploadInfos.size();
    }

    QVariant UploadInfoRepository::data(const QModelIndex &index, int role) const {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return QVariant();
        }

        auto &uploadInfo = m_UploadInfos.at(row);

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
            case PercentRole:
                return uploadInfo->getPercent();
            /*case FtpPassiveModeRole:
                return uploadInfo->getFtpPassiveMode();*/
            case DisableFtpPassiveModeRole:
                return uploadInfo->getDisableFtpPassiveMode();
            case DisableEPSVRole:
                return uploadInfo->getDisableEPSV();
            default:
                return QVariant();
        }
    }

    Qt::ItemFlags UploadInfoRepository::flags(const QModelIndex &index) const {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool UploadInfoRepository::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();

        if (row < 0 || (size_t)row >= m_UploadInfos.size()) {
            return false;
        }

        auto &uploadInfo = m_UploadInfos.at(row);
        QString title;
        int roleToUpdate = 0;
        bool needToUpdate = false;

        switch (role) {
            case EditTitleRole:
                roleToUpdate = TitleRole;
                title = value.toString();
                needToUpdate = uploadInfo->setTitle(title);
                // hack for updating checkbox binding
                if (!needToUpdate) {
                    needToUpdate = true;
                    roleToUpdate = ZipBeforeUploadRole;
                }

                break;
            case EditHostRole:
                roleToUpdate = HostRole;
                needToUpdate = uploadInfo->setHost(value.toString().simplified());
                break;
            case EditUsernameRole:
                roleToUpdate = UsernameRole;
                needToUpdate = uploadInfo->setUsername(value.toString().simplified());
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
            /*case EditFtpPassiveModeRole:
                roleToUpdate = FtpPassiveModeRole;
                needToUpdate = uploadInfo->setFtpPassiveMode(value.toBool());*/
            case EditDisableFtpPassiveModeRole:
                roleToUpdate = DisableFtpPassiveModeRole;
                needToUpdate = uploadInfo->setDisableFtpPassiveMode(value.toBool());
                break;
            case EditDisableEPSVRole:
                roleToUpdate = DisableEPSVRole;
                needToUpdate = uploadInfo->setDisableEPSV(value.toBool());
                break;
            default:
                return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << roleToUpdate);
        }

        return true;
    }

    void UploadInfoRepository::onBeforeMasterPasswordChanged(const QString &oldMasterPassword,
                                                             const QString &newMasterPassword) {
        LOG_INFO << "#";
        m_CommandManager->recodePasswords(oldMasterPassword, newMasterPassword, m_UploadInfos);
    }

    void UploadInfoRepository::onAfterMasterPasswordReset() {
        LOG_INFO << "#";
        for (auto &info: m_UploadInfos) {
            info->dropPassword();
        }
    }

    QHash<int, QByteArray> UploadInfoRepository::roleNames() const {
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
        roles[PercentRole] = "percent";
        /*roles[FtpPassiveModeRole] = "ftppassivemode";
           roles[EditFtpPassiveModeRole] = "editftppassivemode";*/
        roles[DisableFtpPassiveModeRole] = "disablepassivemode";
        roles[EditDisableFtpPassiveModeRole] = "editdisablepassivemode";
        roles[DisableEPSVRole] = "disableEPSV";
        roles[EditDisableEPSVRole] = "editdisableEPSV";
        return roles;
    }

    void UploadInfoRepository::removeInnerItem(int row) {
        m_UploadInfos.erase(m_UploadInfos.begin() + row);
    }
}
