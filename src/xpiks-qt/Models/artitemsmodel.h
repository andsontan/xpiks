#ifndef ARTSITEMSMODEL_H
#define ARTSITEMSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QUrl>
#include "abstractlistmodel.h"
#include "artworkmetadata.h"
#include "artworksrepository.h"
#include "combinedartworksmodel.h"
#include "iptcprovider.h"

namespace Models {
    class ArtItemsModel : public AbstractListModel {
        Q_OBJECT
        Q_PROPERTY(int modifiedArtworksCount READ getModifiedArtworksCount NOTIFY modifiedArtworksCountChanged)
    public:
        ArtItemsModel(QObject *parent = 0) :
            AbstractListModel(parent),
            m_ArtworksRepository(NULL)
        {}

        ~ArtItemsModel();

    public:
        enum ArtItemsRoles {
            ArtworkDescriptionRole = Qt::UserRole + 1,
            EditArtworkDescriptionRole,
            ArtworkFilenameRole,
            ArtworkAuthorRole,
            EditArtworkAuthorRole,
            ArtworkTitleRole,
            EditArtworkTitleRole,
            KeywordsRole,
            KeywordsStringRole,
            IsModifiedRole,
            IsSelectedRole
        };

    public:
        int getModifiedArtworksCount();
        void updateModifiedCount() { emit modifiedArtworksCountChanged(); }

    public:
        void setArtworksRepository(ArtworksRepository *repository) { m_ArtworksRepository = repository; }
        void setCombinedArtworksModel(CombinedArtworksModel *combinedArtworksModel) { m_CombinedArtworks = combinedArtworksModel; }
        void setIptcProvider(IptcProvider *provider) { m_IptcProvider = provider; }

    public:
        Q_INVOKABLE void updateAllProperties();
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);
        Q_INVOKABLE void combineSelectedArtworks() { doCombineSelectedImages(m_CombinedArtworks); }
        Q_INVOKABLE void selectAllArtworks() { setAllItemsSelected(true); }
        Q_INVOKABLE void unselectAllArtworks() { setAllItemsSelected(false); }
        Q_INVOKABLE void setSelectedItemsSaved();
        Q_INVOKABLE int getSelectedItemsCount();
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void updateSelectedArtworks();
        Q_INVOKABLE void patchSelectedArtworks();

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        void addLocalArtworks(const QList<QUrl> &artworksPaths);
        void addLocalDirectory(const QUrl &directory);
        void itemModifiedChanged(bool) { updateModifiedCount(); }

    private:
        void addDirectory(const QString &directory);
        void addFiles(const QStringList &filepath);
        void setAllItemsSelected(bool selected);

    private:
        void doCombineSelectedImages(CombinedArtworksModel *combinedModel) const;

    signals:
        void modifiedArtworksCountChanged();

    protected:
        QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int row);

    private:
        void doRemoveItemsAtIndices(const QList<int> &indicesToRemove);
        void getSelectedItemsIndices(QList<int> &indices);

    private:
        QList<ArtworkMetadata*> m_ArtworkList;
        CombinedArtworksModel *m_CombinedArtworks;
        ArtworksRepository *m_ArtworksRepository;
        IptcProvider *m_IptcProvider;
    };
}

#endif // ARTSITEMSMODEL_H
