#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "artworkmetadata.h"
#include "keywordsmodel.h"

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(QStringList artworkSources READ getArtworkSources WRITE setArtworkSources NOTIFY artworkSourcesChanged)
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
        const QStringList getArtworkSources() const { return m_ArtworksSources; }
        void setArtworkSources(const QStringList& copy);
    signals:
        void artworkSourcesChanged();

    public:
        Q_INVOKABLE void removeArtworkSource(int index);

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
        QStringList m_ArtworksSources;
        QSet<QString> m_ArtworksSourcesSet;
    };
}

#endif // ARTSITEMSMODEL_H
