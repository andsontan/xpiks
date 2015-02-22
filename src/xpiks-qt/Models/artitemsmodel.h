/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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
#include "warningsmanager.h"
#include "artworkuploader.h"
#include "iptcprovider.h"

namespace Models {
    class ArtItemsModel : public AbstractListModel {
        Q_OBJECT
        Q_PROPERTY(int modifiedArtworksCount READ getModifiedArtworksCount NOTIFY modifiedArtworksCountChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)
    public:
        ArtItemsModel(QObject *parent = 0) :
            AbstractListModel(parent),
            m_ArtworksRepository(NULL),
            m_SelectedArtworksCount(0)
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
            KeywordsStringRole,
            KeywordsCountRole,
            IsModifiedRole,
            IsSelectedRole,
            EditIsSelectedRole
        };

    public:
        int getModifiedArtworksCount();
        int getSelectedArtworksCount() { return m_SelectedArtworksCount; }
        void updateModifiedCount() { emit modifiedArtworksCountChanged(); }

    public:
        void setArtworksRepository(ArtworksRepository *repository) { Q_ASSERT(repository != NULL); m_ArtworksRepository = repository; }
        void setCombinedArtworksModel(CombinedArtworksModel *combinedArtworksModel) { Q_ASSERT(combinedArtworksModel != NULL); m_CombinedArtworks = combinedArtworksModel; }
        void setIptcProvider(IptcProvider *provider) { Q_ASSERT(provider != NULL); m_IptcProvider = provider; }
        void setArtworkUploader(ArtworkUploader *uploader) { Q_ASSERT(uploader != NULL); m_ArtworkUploader = uploader; }
        void setWarningsManager(WarningsManager *manager) { Q_ASSERT(manager != NULL); m_WarningsManager = manager; }

    public:
        Q_INVOKABLE void updateAllProperties();
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);
        Q_INVOKABLE void backupItem(int metadataIndex);
        Q_INVOKABLE void combineSelectedArtworks() { doCombineSelectedImages(m_CombinedArtworks); }
        Q_INVOKABLE void combineArtwork(int index) { doCombineArtwork(index, m_CombinedArtworks); }
        Q_INVOKABLE void selectAllArtworks() { setAllItemsSelected(true); }
        Q_INVOKABLE void unselectAllArtworks() { setAllItemsSelected(false); }
        Q_INVOKABLE int dropFiles(const QList<QUrl> &urls);
        Q_INVOKABLE void setSelectedItemsSaved();
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void updateSelectedArtworks();
        Q_INVOKABLE void patchSelectedArtworks();
        Q_INVOKABLE void setSelectedForUpload();
        Q_INVOKABLE bool areSelectedArtworksSaved();
        Q_INVOKABLE bool allArtworksSelected() const { return m_SelectedArtworksCount == m_ArtworkList.length(); }
        Q_INVOKABLE void selectDirectory(int directoryIndex);
        Q_INVOKABLE void checkForWarnings();
        Q_INVOKABLE QObject *getArtworkItself(int index) const {
            ArtworkMetadata *item = NULL;

            if (index >= 0 && index < m_ArtworkList.length()) {
                item = m_ArtworkList[index];
                QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
                qDebug() << "Binding to keywords list... " << item->getFilepath();
            }

            return item;
        }

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);

    public slots:
        int addLocalArtworks(const QList<QUrl> &artworksPaths);
        int addLocalDirectory(const QUrl &directory);
        void itemModifiedChanged(bool) { updateModifiedCount(); }
        void itemSelectedChanged(bool);

    private:
        int addDirectory(const QString &directory);
        int addFiles(const QStringList &filepath);
        void setAllItemsSelected(bool selected);
        void getSelectedArtworks(QList<ArtworkMetadata *> &selectedArtworks) const;

    private:
        void doCombineSelectedImages(CombinedArtworksModel *combinedModel) const;
        void doCombineArtwork(int index, CombinedArtworksModel *combinedModel);

    signals:
        void modifiedArtworksCountChanged();
        void selectedArtworksCountChanged();
        void artworksChanged();

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
        ArtworkUploader *m_ArtworkUploader;
        WarningsManager *m_WarningsManager;
        int m_SelectedArtworksCount;
    };
}

#endif // ARTSITEMSMODEL_H
