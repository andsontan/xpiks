#include <QAbstractListModel>
#include "artworkmetadata.h"
#include "keywordsmodel.h"

#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
    public:
        ArtItemsModel(QObject *parent = 0);

    public:
        enum ArtItemsRoles {
            ImageDescriptionRole = Qt::UserRole + 1,
            ImageFilenameRole,
            KeywordsRole
        };

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
    };
}

#endif // ARTSITEMSMODEL_H
