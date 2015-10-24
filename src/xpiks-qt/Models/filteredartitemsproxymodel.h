/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef FILTEREDARTITEMSPROXYMODEL_H
#define FILTEREDARTITEMSPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
#include <QList>
#include "../Common/baseentity.h"

namespace Models {
    class ArtworkMetadata;
    class ArtItemInfo;
    class ArtItemsModel;

    class FilteredArtItemsProxyModel : public QSortFilterProxyModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
        Q_PROPERTY(int selectedArtworksCount READ getSelectedArtworksCount NOTIFY selectedArtworksCountChanged)
    public:
        FilteredArtItemsProxyModel(QObject *parent=0);

    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);
        int getSelectedArtworksCount() { return m_SelectedArtworksCount; }

    public:
        Q_INVOKABLE int getOriginalIndex(int index);
        Q_INVOKABLE void selectFilteredArtworks() { setFilteredItemsSelected(true); }
        Q_INVOKABLE void unselectFilteredArtworks() { setFilteredItemsSelected(false); }
        Q_INVOKABLE void selectDirectory(int directoryIndex);
        Q_INVOKABLE void combineSelectedArtworks();
        Q_INVOKABLE void setSelectedItemsSaved();
        Q_INVOKABLE void removeSelectedArtworks();
        Q_INVOKABLE void updateSelectedArtworks();
        Q_INVOKABLE void saveSelectedArtworks();
        Q_INVOKABLE void setSelectedForUpload();
        Q_INVOKABLE void setSelectedForZipping();
        Q_INVOKABLE bool areSelectedArtworksSaved();
        Q_INVOKABLE int getModifiedSelectedCount() const;
        Q_INVOKABLE void removeArtworksDirectory(int index);
        Q_INVOKABLE void checkForWarnings();
        Q_INVOKABLE int getItemsCount() const { return rowCount(); }
        Q_INVOKABLE void reimportMetadataForSelected();
        Q_INVOKABLE int findSelectedItemIndex() const;
        Q_INVOKABLE void removeMetadataInSelected() const;
        Q_INVOKABLE void updateFilter() { invalidateFilter(); emit afterInvalidateFilter(); }

    public slots:
        void itemSelectedChanged(bool value);
        void onSelectedArtworksRemoved();

    signals:
        void searchTermChanged(const QString &searchTerm);
        void needCheckItemsForWarnings(const QList<ArtworkMetadata*> &artworks);
        void selectedArtworksCountChanged();
        void afterInvalidateFilter();

    private:
        void setFilteredItemsSelected(bool selected);
        QList<ArtworkMetadata *> getSelectedOriginalItems() const;
        QList<ArtItemInfo *> getSelectedOriginalItemsWithIndices() const;
        QList<int> getSelectedOriginalIndices() const;
        void forceUnselectAllItems();
        ArtItemsModel *getArtItemsModel() const;
        bool fitsSpecialKeywords(const ArtworkMetadata *metadata, bool &isSpecial) const;
        bool containsPartsSearch(const ArtworkMetadata *metadata) const;
        bool containsAnyPartsSearch(const ArtworkMetadata *metadata) const;
        bool containsAllPartsSearch(const ArtworkMetadata *metadata) const;

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const Q_DECL_OVERRIDE;

    private:
        // ignore default regexp from proxymodel
        QString m_SearchTerm;
        volatile int m_SelectedArtworksCount;
    };
}

#endif // FILTEREDARTITEMSPROXYMODEL_H
