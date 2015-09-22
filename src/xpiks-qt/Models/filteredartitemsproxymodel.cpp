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

#include "filteredartitemsproxymodel.h"
#include "artitemsmodel.h"
#include "artworkmetadata.h"
#include "artworksrepository.h"
#include "artiteminfo.h"
#include "../Commands/commandmanager.h"

namespace Models {
    FilteredArtItemsProxyModel::FilteredArtItemsProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent),
        Common::BaseEntity(),
        m_SelectedArtworksCount(0)
    {
    }

    void FilteredArtItemsProxyModel::setSearchTerm(const QString &value) {
        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
            invalidateFilter();
            forceUnselectAllItems();
            emit afterInvalidateFilter();
        }
    }

    int FilteredArtItemsProxyModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    void FilteredArtItemsProxyModel::selectDirectory(int directoryIndex) {
        QList<int> directoryItems;
        ArtItemsModel *artItemsModel = getArtItemsModel();
        const ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        const QString directory = artworksRepository->getDirectory(directoryIndex);

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            Q_ASSERT(metadata != NULL);

            if (metadata->isInDirectory(directory)) {
                directoryItems.append(row);
                metadata->setIsSelected(!metadata->getIsSelected());
            }
        }

        artItemsModel->updateItems(directoryItems, QVector<int>() << ArtItemsModel::IsSelectedRole);
    }

    void FilteredArtItemsProxyModel::combineSelectedArtworks() {
        QList<ArtItemInfo *> artworksList = getSelectedOriginalItemsWithIndices();
        m_CommandManager->combineArtworks(artworksList);
    }

    void FilteredArtItemsProxyModel::setSelectedItemsSaved() {
        QList<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->setSelectedItemsSaved(indices);
    }

    void FilteredArtItemsProxyModel::removeSelectedArtworks() {
        QList<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->removeSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::updateSelectedArtworks() {
        QList<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::saveSelectedArtworks() {
        // former patchSelectedArtworks
        QList<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->saveSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::setSelectedForUpload() {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->setArtworksForUpload(selectedArtworks);
        emit needCheckItemsForWarnings(selectedArtworks);
    }

    void FilteredArtItemsProxyModel::setSelectedForZipping() {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->setArtworksForZipping(selectedArtworks);
    }

    bool FilteredArtItemsProxyModel::areSelectedArtworksSaved() {
        int modifiedSelectedCount = getModifiedSelectedCount();
        return modifiedSelectedCount == 0;
    }

    int FilteredArtItemsProxyModel::getModifiedSelectedCount() const {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        int modifiedCount = 0;

        foreach (const ArtworkMetadata *metadata, selectedArtworks) {
            if (metadata->isModified()) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void FilteredArtItemsProxyModel::removeArtworksDirectory(int index) {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->removeArtworksDirectory(index);
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::checkForWarnings() {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        artItemsModel->checkForWarnings(selectedArtworks);
    }

    void FilteredArtItemsProxyModel::itemSelectedChanged(bool value) {
        int plus = value ? +1 : -1;
        m_SelectedArtworksCount += plus;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::onSelectedArtworksRemoved() {
        m_SelectedArtworksCount--;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::setFilteredItemsSelected(bool selected) {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<int> indices;

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            Q_ASSERT(metadata != NULL);
            metadata->setIsSelected(selected);
            indices << index;
        }

        artItemsModel->updateItems(indices, QVector<int>() << ArtItemsModel::IsSelectedRole);
    }

    QList<ArtworkMetadata *> FilteredArtItemsProxyModel::getSelectedOriginalItems() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<ArtworkMetadata *> selectedArtworks;

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->getIsSelected()) {
                selectedArtworks.append(metadata);
            }
        }

        return selectedArtworks;
    }

    QList<ArtItemInfo *> FilteredArtItemsProxyModel::getSelectedOriginalItemsWithIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<ArtItemInfo *> selectedArtworks;

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->getIsSelected()) {
                ArtItemInfo *info = new ArtItemInfo(metadata, index);
                selectedArtworks.append(info);
            }
        }

        return selectedArtworks;
    }

    QList<int> FilteredArtItemsProxyModel::getSelectedOriginalIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<int> selectedIndices;

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->getIsSelected()) {
                selectedIndices.append(index);
            }
        }

        return selectedIndices;
    }

    void FilteredArtItemsProxyModel::forceUnselectAllItems() {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->forceUnselectAllItems();
        m_SelectedArtworksCount = 0;
        emit selectedArtworksCountChanged();
    }

    ArtItemsModel *FilteredArtItemsProxyModel::getArtItemsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        ArtItemsModel *artItemsModel = dynamic_cast<ArtItemsModel *>(sourceItemModel);
        return artItemsModel;
    }

    bool FilteredArtItemsProxyModel::fitsSpecialKeywords(const ArtworkMetadata *metadata) const {
        if (m_SearchTerm == "x:modified" && metadata->isModified()) {
            return true;
        }

        if (m_SearchTerm == "x:empty" && metadata->isEmpty()) {
            return true;
        }

        if (m_SearchTerm == "x:selected" && metadata->getIsSelected()) {
            return true;
        }

        return false;
    }

    bool FilteredArtItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);


        if (fitsSpecialKeywords(metadata)) { return true; }

        bool hasMatch = false;

        hasMatch = metadata->getDescription().contains(m_SearchTerm, Qt::CaseInsensitive);

        if (!hasMatch) {
            hasMatch = metadata->getTitle().contains(m_SearchTerm, Qt::CaseInsensitive);
        }

        if (!hasMatch) {
            hasMatch = metadata->getFilepath().contains(m_SearchTerm, Qt::CaseInsensitive);
        }

        if (!hasMatch) {
            const QStringList &keywords = metadata->getKeywords();
            foreach (const QString &keyword, keywords) {
                if (keyword.contains(m_SearchTerm, Qt::CaseInsensitive)) {
                    hasMatch = true;
                    break;
                }
            }
        }

        return hasMatch;
    }
}
