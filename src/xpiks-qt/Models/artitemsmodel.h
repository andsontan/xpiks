#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include "artworkmetadata.h"
#include "artworksrepository.h"

namespace Models {
    class ArtItemsModel : public QAbstractListModel {
        Q_OBJECT
        Q_PROPERTY(bool canAddFiles READ getCanAddFiles WRITE setCanAddFiles NOTIFY canAddFilesChanged)
    public:
        ArtItemsModel(QObject *parent = 0) : m_ArtworksRepository(NULL), m_CanAddFiles(true) {}
        ~ArtItemsModel();

    public:
        enum ArtItemsRoles {
            ImageDescriptionRole = Qt::UserRole + 1,
            EditImageDescriptionRole,
            ImageFilenameRole,
            KeywordsRole,
            IsModifiedRole,
            IsSelectedRole
        };

    public:
        void setArtworksRepository(ArtworksRepository *repository) { m_ArtworksRepository = repository; }

    public:
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        void addDirectoryButtonClicked();
        void addFilesButtonClicked();

    private:
        void addDirectory(const QString &directory);
        void addFiles(const QStringList &filepath);
        void removeItems(const QList<QPair<int, int> > &ranges);

    public:
        bool getCanAddFiles () const { return m_CanAddFiles; }
        void setCanAddFiles(bool value) { m_CanAddFiles = value; emit canAddFilesChanged(); }
    signals:
        void canAddFilesChanged();

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        ArtworksRepository *m_ArtworksRepository;
        bool m_CanAddFiles;
    };
}

#endif // ARTSITEMSMODEL_H
