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

namespace Models {
    FilteredArtItemsProxyModel::FilteredArtItemsProxyModel(QObject *parent) :
        QSortFilterProxyModel(parent)
    {
    }

    void FilteredArtItemsProxyModel::setSearchTerm(const QString &value) {
        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
            invalidateFilter();
        }
    }

    int FilteredArtItemsProxyModel::getOriginalIndex(int index) {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    bool FilteredArtItemsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        QAbstractItemModel *sourceItemModel = sourceModel();
        ArtItemsModel *artItemsModel = dynamic_cast<ArtItemsModel *>(sourceItemModel);
        ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        bool hasMatch = false;

        hasMatch = metadata->getDescription().contains(m_SearchTerm, Qt::CaseInsensitive);

        if (!hasMatch) {
            hasMatch = metadata->getAuthor().contains(m_SearchTerm, Qt::CaseInsensitive);
        }

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
