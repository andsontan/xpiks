#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include "abstractlistmodel.h"
#include "artworkmetadata.h"
#include "artworksrepository.h"
#include "combinedartworksmodel.h"

namespace Models {
    class ArtItemsModel : public AbstractListModel {
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
            KeywordsStringRole,
            IsModifiedRole,
            IsSelectedRole
        };

    public:
        void setArtworksRepository(ArtworksRepository *repository) { m_ArtworksRepository = repository; }
        void setCombinedArtworksModel(CombinedArtworksModel *combinedArtworksModel) { m_CombinedArtworks = combinedArtworksModel; }

    public:
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);
        Q_INVOKABLE void combineSelectedArtworks() { doCombineSelectedImages(m_CombinedArtworks); }
        Q_INVOKABLE void selectAllArtworks() { setAllItemsSelected(true); }
        Q_INVOKABLE void unselectAllArtworks() { setAllItemsSelected(false); }
        Q_INVOKABLE int getSelectedItemsCount();

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
        void setAllItemsSelected(bool selected);

    public:
        bool getCanAddFiles () const { return m_CanAddFiles; }
        void setCanAddFiles(bool value) { m_CanAddFiles = value; emit canAddFilesChanged(); }

    private:
        void doCombineSelectedImages(CombinedArtworksModel *combinedModel) const;

    signals:
        void canAddFilesChanged();

    protected:
        QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int row);

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        CombinedArtworksModel *m_CombinedArtworks;
        ArtworksRepository *m_ArtworksRepository;
        bool m_CanAddFiles;
    };
}

#endif // ARTSITEMSMODEL_H
