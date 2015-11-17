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
#include "settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Commands/combinededitcommand.h"
#include "../Common/flags.h"

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
            forceUnselectAllItems();
        }

        invalidateFilter();
        emit afterInvalidateFilter();
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

        QList<ArtItemInfo *> selectedArtworksWithIndices = getSelectedOriginalItemsWithIndices();
        emit needCheckItemsForWarnings(selectedArtworksWithIndices);
    }

    void FilteredArtItemsProxyModel::setSelectedForZipping() {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->setArtworksForZipping(selectedArtworks);
    }

    bool FilteredArtItemsProxyModel::areSelectedArtworksSaved() {
        int modifiedSelectedCount = getModifiedSelectedCount();
        return modifiedSelectedCount == 0;
    }

    void FilteredArtItemsProxyModel::spellCheckSelected() {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->submitForSpellCheck(selectedArtworks);
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
        QList<ArtItemInfo *> selectedArtworks = getSelectedOriginalItemsWithIndices();

        if (selectedArtworks.isEmpty()) {
            selectedArtworks = getAllItemsWithIndices();
        }

        emit needCheckItemsForWarnings(selectedArtworks);
    }

    void FilteredArtItemsProxyModel::reimportMetadataForSelected() {
        QList<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->setArtworksForIPTCProcessing(selectedArtworks);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->raiseArtworksAdded(selectedArtworks.count());
    }

    int FilteredArtItemsProxyModel::findSelectedItemIndex() const {
        int index = -1;
        QList<int> indices = getSelectedOriginalIndices();
        if (indices.length() == 1) {
            index = indices.first();
        }

        return index;
    }

    void FilteredArtItemsProxyModel::removeMetadataInSelected() const {
        QList<ArtItemInfo *> selectedArtworks = getSelectedOriginalItemsWithIndices();

        int flags = 0;
        Common::SetFlag(flags, Common::EditDesctiption);
        Common::SetFlag(flags, Common::EditKeywords);
        Common::SetFlag(flags, Common::EditTitle);

        const QString empty = "";

        Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                    flags,
                    selectedArtworks,
                    empty,
                    empty,
                    QStringList());

        Commands::CommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
        Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
        m_CommandManager->updateArtworks(combinedResult->m_IndicesToUpdate);

        delete combinedResult;
        qDeleteAll(selectedArtworks);
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

    QList<ArtItemInfo *> FilteredArtItemsProxyModel::getAllItemsWithIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QList<ArtItemInfo *> selectedArtworks;

        int size = this->rowCount();
        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL) {
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

    bool FilteredArtItemsProxyModel::fitsSpecialKeywords(const QString &searchTerm, const ArtworkMetadata *metadata) const {
        bool hasMatch = false;

        if (searchTerm == "x:modified") {
            hasMatch = metadata->isModified();
        } else if (searchTerm == "x:empty") {
            hasMatch = metadata->isEmpty();
        } else if (searchTerm == "x:selected") {
            hasMatch = metadata->getIsSelected();
        }

        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::containsPartsSearch(ArtworkMetadata *metadata) const {
        bool hasMatch = false;
        Models::SettingsModel *settings = m_CommandManager->getSettingsModel();

        if (settings->getSearchUsingAnd()) {
            hasMatch = containsAllPartsSearch(metadata);
        } else {
            hasMatch = containsAnyPartsSearch(metadata);
        }

        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::containsAnyPartsSearch(ArtworkMetadata *metadata) const {
        bool hasMatch = false;
        QStringList searchTerms = m_SearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString &description = metadata->getDescription();
        const QString &title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        foreach (const QString &searchTerm, searchTerms) {
            hasMatch = fitsSpecialKeywords(searchTerm, metadata);

            if (!hasMatch) {
                hasMatch = description.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!hasMatch) {
                hasMatch = title.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!hasMatch) {
                hasMatch = filepath.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (hasMatch) { break; }
        }

        if (!hasMatch) {
            foreach (const QString &searchTerm, searchTerms) {
                hasMatch = metadata->containsKeyword(searchTerm);
                if (hasMatch) { break; }
            }
        }

        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::containsAllPartsSearch(ArtworkMetadata *metadata) const {
        bool hasMatch = false;
        QStringList searchTerms = m_SearchTerm.split(QChar::Space, QString::SkipEmptyParts);

        const QString &description = metadata->getDescription();
        const QString &title = metadata->getTitle();
        const QString &filepath = metadata->getFilepath();

        bool anyError = false;

        foreach (const QString &searchTerm, searchTerms) {
            bool anyContains = false;

            anyContains = fitsSpecialKeywords(searchTerm, metadata);

            if (!anyContains) {
                anyContains = description.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                anyContains = title.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                anyContains = filepath.contains(searchTerm, Qt::CaseInsensitive);
            }

            if (!anyContains) {
                anyContains = metadata->containsKeyword(searchTerm);
            }

            if (!anyContains) {
                anyError = true;
                break;
            }
        }

        hasMatch = !anyError;
        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        bool hasMatch = containsPartsSearch(metadata);
        return hasMatch;
    }
}
