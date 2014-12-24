#ifndef UPLOADINFOREPOSITORY_H
#define UPLOADINFOREPOSITORY_H

#include <QAbstractListModel>
#include <QList>
#include "uploadinfo.h"

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

    public:
        QList<UploadInfo*> &getUploadInfos() { return m_UploadInfos; }

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);

    protected:
        QHash<int, QByteArray> roleNames() const;

        void removeInnerItem(int row) {
            UploadInfo *info = m_UploadInfos.takeAt(row);
            delete info;
        }

    private:
        QList<UploadInfo*> m_UploadInfos;
    };
}

#endif // UPLOADINFOREPOSITORY_H
