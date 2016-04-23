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

#include "filteredartitemsproxymodel.h"
#include <QDir>
#include "artitemsmodel.h"
#include "artworkmetadata.h"
#include "artworksrepository.h"
#include "artiteminfo.h"
#include "settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Commands/combinededitcommand.h"
#include "../Common/flags.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"

namespace Models {
    FilteredArtItemsProxyModel::FilteredArtItemsProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent),
        Common::BaseEntity(),
        m_SelectedArtworksCount(0),
        m_SortingEnabled(false)
    {
        //m_SortingEnabled = true;
        //this->sort(0);
    }

    void FilteredArtItemsProxyModel::setSearchTerm(const QString &value) {
        bool anyChangesNeeded = value != m_SearchTerm;
        if (anyChangesNeeded) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
        emit afterInvalidateFilter();
        forceUnselectAllItems();
    }

    void FilteredArtItemsProxyModel::spellCheckAllItems() {
        QVector<ArtworkMetadata *> allArtworks = getAllOriginalItems();
        m_CommandManager->submitForSpellCheck(allArtworks);
        m_CommandManager->reportUserAction(Conectivity::UserActionSpellCheck);
    }

    int FilteredArtItemsProxyModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    void FilteredArtItemsProxyModel::selectDirectory(int directoryIndex) {
        QVector<int> directoryItems;
        int size = this->rowCount();
        directoryItems.reserve(size);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        const ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        const QString &directory = artworksRepository->getDirectory(directoryIndex);
        QDir dir(directory);
        QString directoryAbsolutePath = dir.absolutePath();

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            Q_ASSERT(metadata != NULL);

            if (metadata->isInDirectory(directoryAbsolutePath)) {
                directoryItems.append(index);
                metadata->setIsSelected(!metadata->isSelected());
            }
        }

        emit allItemsSelectedChanged();
        artItemsModel->updateItems(directoryItems, QVector<int>() << ArtItemsModel::IsSelectedRole);
    }

    void FilteredArtItemsProxyModel::combineSelectedArtworks() {
        QVector<ArtItemInfo *> artworksList = getSelectedOriginalItemsWithIndices();
        m_CommandManager->combineArtworks(artworksList);
    }

    void FilteredArtItemsProxyModel::setSelectedItemsSaved() {
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->setSelectedItemsSaved(indices);
    }

    void FilteredArtItemsProxyModel::removeSelectedArtworks() {
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->removeSelectedArtworks(indices);
        updateFilter();
    }

    void FilteredArtItemsProxyModel::updateSelectedArtworks() {
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::saveSelectedArtworks(bool overwriteAll, bool useBackups) {
        // former patchSelectedArtworks
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->saveSelectedArtworks(indices, overwriteAll, useBackups);
    }

    void FilteredArtItemsProxyModel::setSelectedForUpload() {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->setArtworksForUpload(selectedArtworks);
    }

    void FilteredArtItemsProxyModel::setSelectedForZipping() {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        LOG_INFO << selectedArtworks.length() << "item(s)";

        m_CommandManager->setArtworksForZipping(selectedArtworks);
    }

    bool FilteredArtItemsProxyModel::areSelectedArtworksSaved() {
        int modifiedSelectedCount = getModifiedSelectedCount();
        return modifiedSelectedCount == 0;
    }

    void FilteredArtItemsProxyModel::spellCheckSelected() {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->submitForSpellCheck(selectedArtworks);
        m_CommandManager->reportUserAction(Conectivity::UserActionSpellCheck);
    }

    int FilteredArtItemsProxyModel::getModifiedSelectedCount(bool overwriteAll) const {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        int modifiedCount = 0;

        foreach (const ArtworkMetadata *metadata, selectedArtworks) {
            if (metadata->isModified() || overwriteAll) {
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

    void FilteredArtItemsProxyModel::reimportMetadataForSelected() {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        QVector<QPair<int, int> > ranges;
        Helpers::indicesToRanges(getSelectedOriginalIndices(), ranges);

        m_CommandManager->readMetadata(selectedArtworks, ranges);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->raiseArtworksAdded(selectedArtworks.count(), 0);
    }

    int FilteredArtItemsProxyModel::findSelectedItemIndex() const {
        int index = -1;
        QVector<int> indices = getSelectedOriginalIndices();
        if (indices.length() == 1) {
            index = indices.first();
        }

        return index;
    }

    void FilteredArtItemsProxyModel::removeMetadataInSelected() const {
        QVector<ArtItemInfo *> selectedArtworks = getSelectedOriginalItemsWithIndices();
        int flags = 0;
        Common::SetFlag(flags, Common::EditDesctiption);
        Common::SetFlag(flags, Common::EditKeywords);
        Common::SetFlag(flags, Common::EditTitle);
        Common::SetFlag(flags, Common::Clear);
        removeMetadataInItems(selectedArtworks, flags);
    }

    void FilteredArtItemsProxyModel::clearKeywords(int index) {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        int originalIndex = getOriginalIndex(index);
        ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
        Q_ASSERT(metadata != NULL);
        ArtItemInfo *info = new ArtItemInfo(metadata, originalIndex);
        removeKeywordsInItem(info);
    }

    void FilteredArtItemsProxyModel::focusNextItem(int index) {
        if (0 <= index && index < rowCount() - 1) {
            QModelIndex nextQIndex = this->index(index + 1, 0);
            QModelIndex sourceIndex = mapToSource(nextQIndex);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceIndex.row());

            if (metadata != NULL) {
                metadata->requestFocus(+1);
            }
        }
    }

    void FilteredArtItemsProxyModel::focusPreviousItem(int index) {
        if (0 < index && index < rowCount()) {
            QModelIndex nextQIndex = this->index(index - 1, 0);
            QModelIndex sourceIndex = mapToSource(nextQIndex);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceIndex.row());

            if (metadata != NULL) {
                metadata->requestFocus(-1);
            }
        }
    }

    void FilteredArtItemsProxyModel::spellCheckDescription(int index) {
        LOG_DEBUG << "index:" << index;
        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (!metadata->getDescription().trimmed().isEmpty()) {
                m_CommandManager->submitItemForSpellCheck(metadata, Common::SpellCheckDescription);
            } else {
                LOG_INFO << "description is empty";
                metadata->notifySpellCheckResults(Common::SpellCheckDescription);
            }

            m_CommandManager->submitForWarningsCheck(metadata, Common::WarningsCheckDescription);
        }
    }

    void FilteredArtItemsProxyModel::spellCheckTitle(int index) {
        LOG_DEBUG << "index:" << index;
        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (!metadata->getTitle().trimmed().isEmpty()) {
                m_CommandManager->submitItemForSpellCheck(metadata, Common::SpellCheckTitle);
            } else {
                LOG_INFO << "title is empty";
                metadata->notifySpellCheckResults(Common::SpellCheckTitle);
            }

            m_CommandManager->submitForWarningsCheck(metadata, Common::WarningsCheckTitle);
        }
    }

    void FilteredArtItemsProxyModel::toggleSorted() {
        LOG_DEBUG << "current sorted is" << m_SortingEnabled;
        forceUnselectAllItems();

        if (!m_SortingEnabled) {
            m_SortingEnabled = true;
            sort(0);
            invalidate();
        } else {
            m_SortingEnabled = false;
            setSortRole(Qt::InitialSortOrderRole);
            sort(-1);
            invalidate();
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateAllItems();
    }

    void FilteredArtItemsProxyModel::detachVectorFromSelected() {
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->detachVectorsFromSelected(indices);
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

    void FilteredArtItemsProxyModel::onSpellCheckerAvailable(bool afterRestart) {
        if (afterRestart) {
            this->spellCheckAllItems();
        }
    }

    void FilteredArtItemsProxyModel::removeMetadataInItems(const QVector<ArtItemInfo *> &itemsToClear, int flags) const {
        LOG_DEBUG << itemsToClear.length() << "item(s) with flags =" << flags;
        Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                    flags,
                    itemsToClear);

        Commands::ICommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
        Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
        m_CommandManager->updateArtworks(combinedResult->m_IndicesToUpdate);

        delete combinedResult;
        qDeleteAll(itemsToClear);
    }

    void FilteredArtItemsProxyModel::removeKeywordsInItem(ArtItemInfo *itemToClear) {
        int flags = 0;
        Common::SetFlag(flags, Common::EditKeywords);
        Common::SetFlag(flags, Common::Clear);
        removeMetadataInItems(QVector<ArtItemInfo *>() << itemToClear, flags);
    }

    void FilteredArtItemsProxyModel::setFilteredItemsSelected(bool selected) {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<int> indices;
        int size = this->rowCount();
        indices.reserve(size);

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
        emit allItemsSelectedChanged();
    }

    void FilteredArtItemsProxyModel::invertFilteredItemsSelected() {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<int> indices;
        int size = this->rowCount();
        indices.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            Q_ASSERT(metadata != NULL);
            metadata->invertSelection();
            indices << index;
        }

        artItemsModel->updateItems(indices, QVector<int>() << ArtItemsModel::IsSelectedRole);
        emit allItemsSelectedChanged();
    }

    QVector<ArtworkMetadata *> FilteredArtItemsProxyModel::getSelectedOriginalItems() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<ArtworkMetadata *> selectedArtworks;
        int size = this->rowCount();
        selectedArtworks.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->isSelected()) {
                selectedArtworks.append(metadata);
            }
        }

        return selectedArtworks;
    }

    QVector<ArtItemInfo *> FilteredArtItemsProxyModel::getSelectedOriginalItemsWithIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<ArtItemInfo *> selectedArtworks;
        int size = this->rowCount();
        selectedArtworks.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->isSelected()) {
                ArtItemInfo *info = new ArtItemInfo(metadata, index);
                selectedArtworks.append(info);
            }
        }

        return selectedArtworks;
    }

    QVector<ArtItemInfo *> FilteredArtItemsProxyModel::getAllItemsWithIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<ArtItemInfo *> selectedArtworks;
        int size = this->rowCount();
        selectedArtworks.reserve(size);

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

    QVector<ArtworkMetadata *> FilteredArtItemsProxyModel::getAllOriginalItems() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<ArtworkMetadata *> allArtworks;
        int size = this->rowCount();
        allArtworks.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL) {
                allArtworks.append(metadata);
            }
        }

        return allArtworks;
    }

    QVector<int> FilteredArtItemsProxyModel::getSelectedOriginalIndices() const {
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QVector<int> selectedIndices;
        int size = this->rowCount();
        selectedIndices.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && metadata->isSelected()) {
                selectedIndices.append(index);
            }
        }

        return selectedIndices;
    }

    void FilteredArtItemsProxyModel::forceUnselectAllItems() {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->forceUnselectAllItems();
        m_SelectedArtworksCount = 0;
        emit selectedArtworksCountChanged();
        emit allItemsSelectedChanged();
    }

    ArtItemsModel *FilteredArtItemsProxyModel::getArtItemsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        ArtItemsModel *artItemsModel = dynamic_cast<ArtItemsModel *>(sourceItemModel);
        return artItemsModel;
    }

    bool FilteredArtItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        if (m_SearchTerm.trimmed().isEmpty()) { return true; }

        ArtItemsModel *artItemsModel = getArtItemsModel();
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        bool searchUsingAnd = settingsModel->getSearchUsingAnd();

        bool hasMatch = Helpers::containsPartsSearch(m_SearchTerm, metadata, searchUsingAnd);
        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const {
        if (!m_SortingEnabled) {
            return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();

        ArtworkMetadata *leftMetadata = artItemsModel->getArtwork(sourceLeft.row());
        ArtworkMetadata *rightMetadata = artItemsModel->getArtwork(sourceRight.row());

        const QString &leftFilepath = leftMetadata->getFilepath();
        const QString &rightFilepath = rightMetadata->getFilepath();

        QFileInfo leftFI(leftFilepath);
        QFileInfo rightFI(rightFilepath);

        QString leftFilename = leftFI.fileName();
        QString rightFilename = rightFI.fileName();

        int filenamesResult = QString::compare(leftFilename, rightFilename);
        bool result = false;

        if (filenamesResult == 0) {
            result = QString::compare(leftFilepath, rightFilepath) < 0;
        } else {
            result = filenamesResult < 0;
        }

        return result;
    }

#ifdef CORE_TESTS
        int FilteredArtItemsProxyModel::retrieveNumberOfSelectedItems() {
            return getSelectedOriginalIndices().size();
        }
#endif
}
