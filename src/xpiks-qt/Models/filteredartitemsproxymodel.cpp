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
#include "metadataelement.h"
#include "settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Commands/combinededitcommand.h"
#include "../Common/flags.h"
#include "../SpellCheck/ispellcheckable.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"
#include "../Models/previewmetadataelement.h"

namespace Models {
    FilteredArtItemsProxyModel::FilteredArtItemsProxyModel(QObject *parent):
        QSortFilterProxyModel(parent),
        Common::BaseEntity(),
        m_SelectedArtworksCount(0),
        m_SortingEnabled(false) {
        // m_SortingEnabled = true;
        // this->sort(0);
    }

    void FilteredArtItemsProxyModel::setSearchTerm(const QString &value) {
        LOG_INFO << value;
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
        LOG_DEBUG << "#";
        QVector<ArtworkMetadata *> allArtworks = getAllOriginalItems();
        m_CommandManager->submitForSpellCheck(allArtworks);
        m_CommandManager->reportUserAction(Conectivity::UserAction::SpellCheck);
    }

    int FilteredArtItemsProxyModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();

        return row;
    }

    void FilteredArtItemsProxyModel::selectDirectory(int directoryIndex) {
        LOG_DEBUG << "directory index:" << directoryIndex;

        QVector<int> directoryItems;
        int size = this->rowCount();
        directoryItems.reserve(size);

        ArtItemsModel *artItemsModel = getArtItemsModel();
        const ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        const QString &directory = artworksRepository->getDirectory(directoryIndex);

        LOG_DEBUG << directory;

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
        LOG_DEBUG << "#";
        auto artworksList = getSelectedOriginalItemsWithIndices();
        m_CommandManager->combineArtworks(artworksList);
    }

    void FilteredArtItemsProxyModel::setSelectedItemsSaved() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->setSelectedItemsSaved(indices);
    }

    void FilteredArtItemsProxyModel::removeSelectedArtworks() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->removeSelectedArtworks(indices);
        updateFilter();
    }

    void FilteredArtItemsProxyModel::updateSelectedArtworks() {
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->updateSelectedArtworks(indices);
    }

    void FilteredArtItemsProxyModel::saveSelectedArtworks(bool overwriteAll, bool useBackups) {
        LOG_INFO << "ovewriteAll:" << overwriteAll << "useBackups:" << useBackups;
        // former patchSelectedArtworks
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->saveSelectedArtworks(indices, overwriteAll, useBackups);
    }

    void FilteredArtItemsProxyModel::setSelectedForUpload() {
        LOG_DEBUG << "#";
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
        LOG_DEBUG << "#";
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        m_CommandManager->submitForSpellCheck(selectedArtworks);
        m_CommandManager->reportUserAction(Conectivity::UserAction::SpellCheck);
    }

    int FilteredArtItemsProxyModel::getModifiedSelectedCount(bool overwriteAll) const {
        QVector<ArtworkMetadata *> selectedArtworks = getSelectedOriginalItems();
        int modifiedCount = 0;

        foreach(const ArtworkMetadata *metadata, selectedArtworks) {
            if (metadata->isModified() || overwriteAll) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void FilteredArtItemsProxyModel::removeArtworksDirectory(int index) {
        LOG_DEBUG << "#";
        ArtItemsModel *artItemsModel = getArtItemsModel();

        artItemsModel->removeArtworksDirectory(index);
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::deleteKeywordsFromSelected() {
        auto selectedItems = getSelectedOriginalItemsWithIndices();

        m_CommandManager->deleteKeywordsFromArtworks(selectedItems);
    }

    void FilteredArtItemsProxyModel::reimportMetadataForSelected() {
        LOG_DEBUG << "#";
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
        LOG_DEBUG << "#";
        auto selectedArtworks = getSelectedOriginalItemsWithIndices();
        Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;
        using namespace Common;
        Common::SetFlag(flags, CombinedEditFlags::EditDesctiption);
        Common::SetFlag(flags, CombinedEditFlags::EditKeywords);
        Common::SetFlag(flags, CombinedEditFlags::EditTitle);
        Common::SetFlag(flags, CombinedEditFlags::Clear);
        removeMetadataInItems(selectedArtworks, flags);
    }

    void FilteredArtItemsProxyModel::clearKeywords(int index) {
        LOG_INFO << "index:" << index;
        ArtItemsModel *artItemsModel = getArtItemsModel();
        int originalIndex = getOriginalIndex(index);
        ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);

        if ((metadata != NULL) && (!metadata->areKeywordsEmpty())) {
            removeKeywordsInItem(metadata, originalIndex);
        }
    }

    void FilteredArtItemsProxyModel::focusNextItem(int index) {
        LOG_INFO << "index:" << index;
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
        LOG_INFO << "index:" << index;
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
            if (metadata != NULL) {
                Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();

                if (!metadata->getDescription().trimmed().isEmpty()) {
                    m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Description);
                } else {
                    LOG_INFO << "description is empty";
                    keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Description);
                }

                m_CommandManager->submitForWarningsCheck(metadata, Common::WarningsCheckFlags::Description);
            }
        }
    }

    void FilteredArtItemsProxyModel::spellCheckTitle(int index) {
        LOG_DEBUG << "index:" << index;
        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (metadata != NULL) {
                Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();

                if (!metadata->getTitle().trimmed().isEmpty()) {
                    m_CommandManager->submitItemForSpellCheck(keywordsModel, Common::SpellCheckFlags::Title);
                } else {
                    LOG_INFO << "title is empty";
                    keywordsModel->notifySpellCheckResults(Common::SpellCheckFlags::Title);
                }

                m_CommandManager->submitForWarningsCheck(metadata, Common::WarningsCheckFlags::Title);
            }
        }
    }

    void FilteredArtItemsProxyModel::toggleSorted() {
        LOG_INFO << "current sorted is" << m_SortingEnabled;
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
        LOG_DEBUG << "#";
        QVector<int> indices = getSelectedOriginalIndices();
        ArtItemsModel *artItemsModel = getArtItemsModel();
        artItemsModel->detachVectorsFromSelected(indices);
    }

    QObject *FilteredArtItemsProxyModel::getArtworkMetadata(int index) {
        int originalIndex = getOriginalIndex(index);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QObject *item = artItemsModel->getArtworkMetadata(originalIndex);

        return item;
    }

    QObject *FilteredArtItemsProxyModel::getKeywordsModel(int index) {
        int originalIndex = getOriginalIndex(index);
        ArtItemsModel *artItemsModel = getArtItemsModel();
        QObject *item = artItemsModel->getKeywordsModel(originalIndex);

        return item;
    }

    bool FilteredArtItemsProxyModel::hasTitleWordSpellError(int index, const QString &word) {
        bool result = false;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (metadata != NULL) {
                Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();
                result = keywordsModel->hasTitleWordSpellError(word);
            }
        }

        return result;
    }

    bool FilteredArtItemsProxyModel::hasDescriptionWordSpellError(int index, const QString &word) {
        bool result = false;

        if (0 <= index && index < rowCount()) {
            int originalIndex = getOriginalIndex(index);
            LOG_INFO << originalIndex;
            ArtItemsModel *artItemsModel = getArtItemsModel();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(originalIndex);
            if (metadata != NULL) {
                Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();
                result = keywordsModel->hasDescriptionWordSpellError(word);
            }
        }

        return result;
    }

    void FilteredArtItemsProxyModel::itemSelectedChanged(bool value) {
        int plus = value ? +1 : -1;

        m_SelectedArtworksCount += plus;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::onSelectedArtworksRemoved(int value) {
        m_SelectedArtworksCount -= value;
        emit selectedArtworksCountChanged();
    }

    void FilteredArtItemsProxyModel::onSpellCheckerAvailable(bool afterRestart) {
        LOG_INFO << "after restart:" << afterRestart;
        if (afterRestart) {
            this->spellCheckAllItems();
        }
    }

    void FilteredArtItemsProxyModel::removeMetadataInItems(std::vector<MetadataElement> &itemsToClear, Common::CombinedEditFlags flags) const {
        LOG_INFO << itemsToClear.size() << "item(s) with flags =" << (int)flags;
        std::shared_ptr<Commands::CombinedEditCommand> combinedEditCommand(new Commands::CombinedEditCommand(
                flags,
                itemsToClear));

        m_CommandManager->processCommand(combinedEditCommand);
    }

    void FilteredArtItemsProxyModel::removeKeywordsInItem(ArtworkMetadata *metadata, int originalIndex) {
        Common::CombinedEditFlags flags = Common::CombinedEditFlags::None;
        Common::SetFlag(flags, Common::CombinedEditFlags::EditKeywords);
        Common::SetFlag(flags, Common::CombinedEditFlags::Clear);

        std::vector<MetadataElement> items;

        items.emplace_back(metadata, originalIndex);

        removeMetadataInItems(items, flags);
    }

    void FilteredArtItemsProxyModel::setFilteredItemsSelected(bool selected) {
        LOG_INFO << selected;
        ArtItemsModel *artItemsModel = getArtItemsModel();

        QVector<int> indices;
        int size = this->rowCount();
        indices.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
            if (metadata != NULL) {
                metadata->setIsSelected(selected);
                indices << index;
            }
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
            if (metadata != NULL) {
                metadata->invertSelection();
                indices << index;
            }
        }

        artItemsModel->updateItems(indices, QVector<int>() << ArtItemsModel::IsSelectedRole);
        emit allItemsSelectedChanged();
    }

    QVector<ArtworkMetadata *> FilteredArtItemsProxyModel::getSelectedOriginalItems() const {
        std::vector<ArtworkMetadata *> items = getFilteredOriginalItems<ArtworkMetadata *>(
            [](ArtworkMetadata *metadata) { return metadata->isSelected(); },
            [] (ArtworkMetadata *metadata, int) { return metadata; });

        return QVector<ArtworkMetadata *>::fromStdVector(items);
    }

    std::vector<MetadataElement> FilteredArtItemsProxyModel::getSelectedOriginalItemsWithIndices() const {
        return getFilteredOriginalItems<MetadataElement>(
            [](ArtworkMetadata *artwork) { return artwork->isSelected(); },
            [] (ArtworkMetadata *metadata, int index) { return MetadataElement(metadata, index); });
    }

    std::vector<MetadataElement> FilteredArtItemsProxyModel::getAllItemsWithIndices() const {
        return getFilteredOriginalItems<MetadataElement>(
            [](ArtworkMetadata *) { return true; },
            [] (ArtworkMetadata *metadata, int index) { return MetadataElement(metadata, index); });
    }

    template<typename T>
    std::vector<T> FilteredArtItemsProxyModel::getFilteredOriginalItems(std::function<bool (ArtworkMetadata *)> pred,
                                                                        std::function<T(ArtworkMetadata *, int)> mapper) const {
        ArtItemsModel *artItemsModel = getArtItemsModel();

        std::vector<T> filteredArtworks;
        int size = this->rowCount();

        filteredArtworks.reserve(size);

        for (int row = 0; row < size; ++row) {
            QModelIndex proxyIndex = this->index(row, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);

            int index = originalIndex.row();
            ArtworkMetadata *metadata = artItemsModel->getArtwork(index);

            if (metadata != NULL && pred(metadata)) {
                filteredArtworks.push_back(mapper(metadata, index));
            }
        }

        LOG_INFO << "Filtered" << filteredArtworks.size() << "item(s)";

        return filteredArtworks;
    }

    QVector<ArtworkMetadata *> FilteredArtItemsProxyModel::getAllOriginalItems() const {
        std::vector<ArtworkMetadata *> items = getFilteredOriginalItems<ArtworkMetadata *>(
            [](ArtworkMetadata *) { return true; },
            [] (ArtworkMetadata *metadata, int) { return metadata; });

        return QVector<ArtworkMetadata *>::fromStdVector(items);
    }

    QVector<int> FilteredArtItemsProxyModel::getSelectedOriginalIndices() const {
        std::vector<int> items = getFilteredOriginalItems<int>(
            [](ArtworkMetadata *metadata) { return metadata->isSelected(); },
            [] (ArtworkMetadata *, int index) { return index; });

        return QVector<int>::fromStdVector(items);
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

        if (m_SearchTerm.trimmed().isEmpty()) {
            return true;
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        bool hasMatch = false;

        if (metadata != NULL) {
            SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
            bool searchUsingAnd = settingsModel->getSearchUsingAnd();

            hasMatch = Helpers::containsPartsSearch(m_SearchTerm, metadata, searchUsingAnd);
        }

        return hasMatch;
    }

    bool FilteredArtItemsProxyModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const {
        if (!m_SortingEnabled) {
            return QSortFilterProxyModel::lessThan(sourceLeft, sourceRight);
        }

        ArtItemsModel *artItemsModel = getArtItemsModel();

        ArtworkMetadata *leftMetadata = artItemsModel->getArtwork(sourceLeft.row());
        ArtworkMetadata *rightMetadata = artItemsModel->getArtwork(sourceRight.row());

        bool result = false;

        if (leftMetadata != NULL && rightMetadata != NULL) {
            const QString &leftFilepath = leftMetadata->getFilepath();
            const QString &rightFilepath = rightMetadata->getFilepath();

            QFileInfo leftFI(leftFilepath);
            QFileInfo rightFI(rightFilepath);

            QString leftFilename = leftFI.fileName();
            QString rightFilename = rightFI.fileName();

            int filenamesResult = QString::compare(leftFilename, rightFilename);

            if (filenamesResult == 0) {
                result = QString::compare(leftFilepath, rightFilepath) < 0;
            } else {
                result = filenamesResult < 0;
            }
        }

        return result;
    }

#ifdef CORE_TESTS
    int FilteredArtItemsProxyModel::retrieveNumberOfSelectedItems() {
        return getSelectedOriginalIndices().size();
    }

#endif

    std::vector<MetadataElement> FilteredArtItemsProxyModel::getSearchableOriginalItems(const QString &searchTerm, Common::SearchFlags flags) const {
        return getFilteredOriginalItems<MetadataElement>(
            [&searchTerm, flags](ArtworkMetadata *artwork) {
            return Helpers::hasSearchMatch(searchTerm, artwork, flags);
        },
            [] (ArtworkMetadata *metadata, int index) { return MetadataElement(metadata, index); });
    }

    std::vector<PreviewMetadataElement> FilteredArtItemsProxyModel::getSearchablePreviewOriginalItems(const QString &searchTerm, Common::SearchFlags
                                                                                                      flags) const {
        return getFilteredOriginalItems<PreviewMetadataElement>(
            [&searchTerm, flags](ArtworkMetadata *artwork) {
            return Helpers::hasSearchMatch(searchTerm, artwork, flags);
        },
            [] (ArtworkMetadata *metadata, int index) { return PreviewMetadataElement(metadata, index); });
    }
}
