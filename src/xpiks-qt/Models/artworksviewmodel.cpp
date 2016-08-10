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

#include "artworksviewmodel.h"
#include <QVector>
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"

namespace Models {
    ArtworksViewModel::ArtworksViewModel(QObject *parent):
        AbstractListModel(parent)
    {
    }

    void ArtworksViewModel::setArtworks(std::vector<MetadataElement> &artworks) {
        LOG_INFO << artworks.size() << "artworks";
        size_t paramLength = artworks.size();

        if (paramLength > 0) {
            beginResetModel();
            m_ArtworksList = std::move(artworks);
            endResetModel();

            emit artworksCountChanged();
        }
    }

    int ArtworksViewModel::getSelectedArtworksCount() const {
        int selectedCount = 0;
        size_t count = m_ArtworksList.size();
        for (size_t i = 0; i < count; ++i) {
            if (m_ArtworksList.at(i).isSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    void ArtworksViewModel::setArtworkSelected(int index, bool value) {
        if (index < 0 || index >= (int)m_ArtworksList.size()) {
            return;
        }

        m_ArtworksList.at(index).setSelected(value);
        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        emit selectedArtworksCountChanged();
    }

    ArtworkMetadata *ArtworksViewModel::getArtworkMetadata(size_t i) const {
        Q_ASSERT((i >= 0) && (i < m_ArtworksList.size()));
        return m_ArtworksList.at(i).getOrigin();
    }

    /*virtual*/
    bool ArtworksViewModel::doRemoveSelectedArtworks() {
        LOG_DEBUG << "#";

        int count = (int)m_ArtworksList.size();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve(count);

        for (int i = 0; i < count; ++i) {
            const MetadataElement &item = m_ArtworksList.at(i);
            if (item.isSelected()) {
                indicesToRemove.append(i);
            }
        }

        bool anyItemToRemove = !indicesToRemove.empty();

        if (anyItemToRemove) {
            LOG_INFO << "Removing" << indicesToRemove.size() << "item(s)";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);

            if (m_ArtworksList.empty()) {
                emit requestCloseWindow();
            }

            emit artworksCountChanged();
        }

        return anyItemToRemove;
    }

    void ArtworksViewModel::doResetModel() {
        LOG_DEBUG << "#";

        beginResetModel();
        m_ArtworksList.clear();
        endResetModel();
    }

    void ArtworksViewModel::processArtworks(std::function<bool (const MetadataElement &)> pred,
                                            std::function<void (int, ArtworkMetadata *)> action) const {
        LOG_DEBUG << "#";
        int index = 0;

        for (auto &item: m_ArtworksList) {
            if (pred(item)) {
                action(index, item.getOrigin());
            }

            index++;
        }
    }

    int ArtworksViewModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ArtworksList.size();
    }

    QVariant ArtworksViewModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= (int)m_ArtworksList.size()) { return QVariant(); }

        auto &item = m_ArtworksList.at(row);
        auto *artwork = item.getOrigin();

        switch (role) {
        case FilepathRole: return artwork->getFilepath();
        case IsSelectedRole: return item.isSelected();
        default: return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworksViewModel::roleNames() const {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();
        names[FilepathRole] = "filepath";
        names[IsSelectedRole] = "isselected";
        return names;
    }

    bool ArtworksViewModel::removeUnavailableItems() {
        LOG_DEBUG << "#";

        bool anyUnavailable = false;
        QVector<int> indicesToRemove;
        size_t size = m_ArtworksList.size();

        for (size_t i = 0; i < size; i++) {
            MetadataElement &item = m_ArtworksList.at(i);

            if (item.getOrigin()->isUnavailable()) {
                indicesToRemove.append((int)i);
                anyUnavailable = true;
            }
        }

        if (anyUnavailable) {
            LOG_INFO << "Found" << indicesToRemove.length() << "unavailable item(s)";
            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);

            removeItemsAtIndices(rangesToRemove);

            if (m_ArtworksList.empty()) {
                emit requestCloseWindow();
            }

            emit artworksCountChanged();
        }

        return anyUnavailable;
    }

    void ArtworksViewModel::removeInnerItem(int row) {
        m_ArtworksList.erase(m_ArtworksList.begin() + row);
    }
}
