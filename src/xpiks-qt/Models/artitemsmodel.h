#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "artworkmetadata.h"
#include "keywordsmodel.h"

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(QStringList directoryList READ getDirectoryList WRITE setDirectoryList NOTIFY directoryListChanged)
    public:
        ArtItemsModel(QObject *parent = 0);

    public:
        enum ArtItemsRoles {
            ImageDescriptionRole = Qt::UserRole + 1,
            ImageFilenameRole,
            KeywordsRole
        };

    public:
        const QStringList getDirectoryList() const { return m_DirectoryList; }
        void setDirectoryList(const QStringList& copy);
    signals:
        void directoryListChanged();

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    public slots:
        void addDirectoryButtonClicked();

    private:
        void addDirectory(const QString &directory);

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QList<ArtworkMetadata> m_ArtworkList;
        QStringList m_DirectoryList;
    };
}

#endif // ARTSITEMSMODEL_H
