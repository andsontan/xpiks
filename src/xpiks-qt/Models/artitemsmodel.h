#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "artworkmetadata.h"
#include "keywordsmodel.h"
#include "Models/artworksdirectories.h"

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(ArtworksDirectories artworksDirectories READ getArtworksDirectories WRITE setArtworksDirectoires NOTIFY artworksDirectoriesChanged)
    public:
        ArtItemsModel(QObject *parent = 0);
        ~ArtItemsModel();

    public:
        enum ArtItemsRoles {
            ImageDescriptionRole = Qt::UserRole + 1,
            ImageFilenameRole,
            KeywordsRole
        };

    public:
        const ArtworksDirectories &getArtworksDirectories() const { return m_ArtworksDirectories; }
        void setArtworksDirectories(const ArtworksDirectories& copy);
    signals:
        void artworksDirectoriesChanged();

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
        void addFiles(const QString &filepath);

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        ArtworksDirectories m_ArtworksDirectories;
    };
}

#endif // ARTSITEMSMODEL_H
