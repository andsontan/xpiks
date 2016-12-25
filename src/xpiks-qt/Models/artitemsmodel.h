/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
#include <QVector>
#include <QUrl>
#include <QSize>
#include <QHash>
#include <QQuickTextDocument>
#include <QVector>
#include <deque>
#include "../Common/abstractlistmodel.h"
#include "../Common/baseentity.h"
#include "../Common/ibasicartwork.h"
#include "../Common/iartworkssource.h"
#include "../Helpers/ifilenotavailablemodel.h"

namespace Common {
    class BasicMetadataModel;
}

namespace Models {
    class ArtworkMetadata;
    class MetadataElement;

    class ArtItemsModel:
        public Common::AbstractListModel,
        public Common::BaseEntity,
        public Common::IArtworksSource,
        public Helpers::IFileNotAvailableModel
    {
    Q_OBJECT
    Q_PROPERTY(int modifiedArtworksCount READ getModifiedArtworksCount NOTIFY modifiedArtworksCountChanged)

    public:
        ArtItemsModel(QObject *parent=0);

        virtual ~ArtItemsModel();

    public:
        enum ArtItemsModel_Roles {
            ArtworkDescriptionRole = Qt::UserRole + 1,
            EditArtworkDescriptionRole,
            ArtworkFilenameRole,
            ArtworkTitleRole,
            EditArtworkTitleRole,
            KeywordsStringRole,
            KeywordsCountRole,
            IsModifiedRole,
            IsSelectedRole,
            EditIsSelectedRole,
            HasVectorAttachedRole,
            BaseFilenameRole
        };

    public:
        virtual ArtworkMetadata *createMetadata(const QString &filepath);
        void deleteAllItems();

    public:
        int getModifiedArtworksCount();

        void updateModifiedCount() { emit modifiedArtworksCountChanged(); }
        void updateItems(const QVector<int> &indices, const QVector<int> &roles);
        void forceUnselectAllItems() const;
        Q_INVOKABLE void updateAllItems();
        virtual bool removeUnavailableItems();
        void generateAboutToBeRemoved();

    public:
        /*Q_INVOKABLE*/ void removeArtworksDirectory(int index);
        Q_INVOKABLE void removeKeywordAt(int metadataIndex, int keywordIndex);
        Q_INVOKABLE void removeLastKeyword(int metadataIndex);
        Q_INVOKABLE void appendKeyword(int metadataIndex, const QString &keyword);
        Q_INVOKABLE void pasteKeywords(int metadataIndex, const QStringList &keywords);
        Q_INVOKABLE void addSuggestedKeywords(int metadataIndex, const QStringList &keywords);
        Q_INVOKABLE void suggestCorrections(int metadataIndex);
        Q_INVOKABLE void backupItem(int metadataIndex);

        Q_INVOKABLE void combineArtwork(int index) { doCombineArtwork(index); }
        Q_INVOKABLE int dropFiles(const QList<QUrl> &urls);

        /*Q_INVOKABLE*/ void setSelectedItemsSaved(const QVector<int> &selectedIndices);

        /*Q_INVOKABLE*/ void removeSelectedArtworks(QVector<int> &selectedIndices);

        /*Q_INVOKABLE*/ void updateSelectedArtworks(const QVector<int> &selectedIndices);

        /*Q_INVOKABLE*/ void saveSelectedArtworks(const QVector<int> &selectedIndices, bool overwriteAll, bool useBackups);

        /*Q_INVOKABLE*/ ArtworkMetadata *getArtworkMetadata(int index) const;

        /*Q_INVOKABLE*/ Common::BasicMetadataModel *getBasicModel(int index) const;

        Q_INVOKABLE QSize retrieveImageSize(int metadataIndex) const;
        Q_INVOKABLE QString retrieveFileSize(int metadataIndex) const;
        Q_INVOKABLE QString getArtworkFilepath(int index) const;
        Q_INVOKABLE QString getAttachedVectorPath(int metadataIndex) const;
        Q_INVOKABLE QString getArtworkDateTaken(int metadataIndex) const;

        Q_INVOKABLE int addRecentDirectory(const QString &directory);
        Q_INVOKABLE void initDescriptionHighlighting(int metadataIndex, QQuickTextDocument *document);
        Q_INVOKABLE void initTitleHighlighting(int metadataIndex, QQuickTextDocument *document);

        Q_INVOKABLE void editKeyword(int metadataIndex, int keywordIndex, const QString &replacement);
        Q_INVOKABLE void plainTextEdit(int metadataIndex, const QString &rawKeywords);

        /*Q_INVOKABLE*/ void detachVectorsFromSelected(const QVector<int> &selectedIndices);

    public:
        virtual int rowCount(const QModelIndex &parent=QModelIndex()) const;
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

    public slots:
        int addLocalArtworks(const QList<QUrl> &artworksPaths);
        int addLocalDirectories(const QList<QUrl> &directories);

        void itemModifiedChanged(bool) { updateModifiedCount(); }
        void spellCheckErrorsChanged();
        void onFilesUnavailableHandler();
        void artworkBackupRequested();
        void onUndoStackEmpty();
        void userDictUpdateHandler(const QStringList &keywords);
        void userDictClearedHandler();

    public:
        virtual void removeItemsAtIndices(const QVector<QPair<int, int> > &ranges);
        void beginAccountingFiles(int filesCount);
        void beginAccountingFiles(int start, int end);
        void endAccountingFiles();
        void beginAccountingManyFiles();
        void endAccountingManyFiles();

    public:
        void insertArtwork(int index, ArtworkMetadata *metadata);
        void appendMetadata(ArtworkMetadata *metadata);
        void removeArtworks(const QVector<QPair<int, int> > &ranges);
        ArtworkMetadata *getArtwork(int index) const;
        void raiseArtworksAdded(int imagesCount, int vectorsCount);
        virtual void updateItemsAtIndices(const QVector<int> &indices);
        virtual void updateItemsInRanges(const QVector<QPair<int, int> > &ranges);
        void setAllItemsSelected(bool selected);
        int attachVectors(const QHash<QString, QHash<QString, QString> > &vectorsPaths, QVector<int> &indicesToUpdate) const;

    public:
        // IARTWORKSSOURCE
        virtual Common::IBasicArtwork *getBasicArtwork(int index) const;

        virtual int getArtworksCount() const { return (int)m_ArtworkList.size(); }

    private:
        void updateItemAtIndex(int metadataIndex);
        int addDirectories(const QStringList &directories);
        void doAddDirectory(const QString &directory, QStringList &filesList);
        int addFiles(const QStringList &filepath);

    private:
        void doCombineArtwork(int index);

    signals:
        void modifiedArtworksCountChanged();
        void artworksChanged(bool needToMoveCurrentItem);
        void artworksAdded(int imagesCount, int vectorsCount);
        void selectedArtworksRemoved(int count);
        void fileWithIndexUnavailable(int index);
        void unavailableArtworksFound();
        void unavailableVectorsFound();
        void userDictUpdate(const QString &word);

    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual bool shouldRemoveInRanges(int rangesLength) const;
        virtual void removeInnerItem(int row);
        virtual void removeInnerItemRange(int start, int end);

    private:
        void destroyInnerItem(ArtworkMetadata *metadata);
        void doRemoveItemsAtIndices(QVector<int> &indicesToRemove);
        void doRemoveItemsInRanges(const QVector<QPair<int, int> > &rangesToRemove);
        void getSelectedItemsIndices(QVector<int> &indices);

    private:
        void fillStandardRoles(QVector<int> &roles) const;

#ifdef CORE_TESTS

    public:
        const std::deque<ArtworkMetadata *> &getFinalizationList() const { return m_FinalizationList; }
#endif

    private:
        std::deque<ArtworkMetadata *> m_ArtworkList;
        std::deque<ArtworkMetadata *> m_FinalizationList;
        qint64 m_LastID;
    };
}

#endif // ARTSITEMSMODEL_H
