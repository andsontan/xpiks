#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "artworkmetadata.h"
#include "keywordsmodel.h"
#include "artworksrepository.h"

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
    public:
        ArtItemsModel(QObject *parent = 0) : m_ArtworksRepository(new ArtworksRepository()) {}
        ~ArtItemsModel();

    public:
        enum ArtItemsRoles {
            ImageDescriptionRole = Qt::UserRole + 1,
            ImageFilenameRole,
            KeywordsRole
        };

    public:
        Q_INVOKABLE QObject* getArtworksRepository()
        {
            return m_ArtworksRepository;
        }

    public:
        Q_INVOKABLE void removeArtworksDirectory(int index);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    public slots:
        void addDirectoryButtonClicked();
        void addFilesButtonClicked();

    private:
        void addDirectory(const QString &directory);
        void addFiles(const QStringList &filepath);

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        ArtworksRepository *m_ArtworksRepository;
    };
}

#endif // ARTSITEMSMODEL_H
