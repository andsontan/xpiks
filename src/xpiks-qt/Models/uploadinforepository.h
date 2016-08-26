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

#ifndef UPLOADINFOREPOSITORY_H
#define UPLOADINFOREPOSITORY_H

#include <QAbstractListModel>
#include <QDataStream>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <vector>
#include <memory>
#include "../Common/baseentity.h"

namespace Models {
    class UploadInfo;

    class UploadInfoRepository:
        public QAbstractListModel, public Common::BaseEntity
    {
    Q_OBJECT
    Q_PROPERTY(int infosCount READ getInfosCount NOTIFY infosCountChanged)

    public:
        UploadInfoRepository(QObject *parent = 0):
            QAbstractListModel(parent),
            Common::BaseEntity(),
            m_EmptyPasswordsMode(false)
        {}

        virtual ~UploadInfoRepository();

        void initFromString(const QString &savedString);

    public:
        enum UploadInfoRepository_Roles {
            TitleRole = Qt::UserRole + 1,
            EditTitleRole,
            HostRole,
            EditHostRole,
            UsernameRole,
            EditUsernameRole,
            PasswordRole,
            EditPasswordRole,
            IsSelectedRole,
            EditIsSelectedRole,
            ZipBeforeUploadRole,
            EditZipBeforeUploadRole,
            EditUploadDirectoryRole,
            PercentRole,
            /*DEPRECATED*/ FtpPassiveModeRole,
            /*DEPRECATED*/ EditFtpPassiveModeRole,
            DisableFtpPassiveModeRole,
            EditDisableFtpPassiveModeRole,
            DisableEPSVRole,
            EditDisableEPSVRole
        };

        int getInfosCount() const { return (int)m_UploadInfos.size(); }

    signals:
        void infosCountChanged();

    public:
        Q_INVOKABLE void removeItem(int row) {
            beginRemoveRows(QModelIndex(), row, row);
            removeInnerItem(row);
            endRemoveRows();
            emit infosCountChanged();
        }

        Q_INVOKABLE void addItem();
        Q_INVOKABLE QString getInfoString() const;
        Q_INVOKABLE int getSelectedInfosCount() const;
        Q_INVOKABLE QString getAgenciesWithMissingDetails();
        Q_INVOKABLE void updateProperties(int itemIndex);

        Q_INVOKABLE void initializeAccounts(bool mpIsCorrectOrEmpty);
        Q_INVOKABLE void finalizeAccounts();

        Q_INVOKABLE bool isMasterPasswordCorrectOrEmpty() const { return !m_EmptyPasswordsMode; }

    public:
        void setEmptyPasswordsMode(bool mode) { m_EmptyPasswordsMode = mode; }
        void backupAndDropRealPasswords();
        void restoreRealPasswords();

    public:
        void updatePercentages();
        void resetPercents();

    public:
        const std::vector<std::shared_ptr<UploadInfo > > &getUploadInfos() const { return m_UploadInfos; }
        std::vector<std::shared_ptr<UploadInfo> > retrieveSelectedUploadInfos() const;

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    public slots:
        void onBeforeMasterPasswordChanged(const QString &oldMasterPassword, const QString &newMasterPassword);
        void onAfterMasterPasswordReset();

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

        void removeInnerItem(int row);

    private:
        std::vector<std::shared_ptr<UploadInfo> > m_UploadInfos;
        // when MP is cancelled before Upload dialog
        // all passwords should be empty
        bool m_EmptyPasswordsMode;
    };
}

#endif // UPLOADINFOREPOSITORY_H
