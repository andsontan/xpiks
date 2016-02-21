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

#include "warningsmodel.h"
#include <QStringList>
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"

namespace Warnings {
    void describeWarningFlags(int warningsFlags, QStringList &descriptions) {

        if (Common::HasFlag(warningsFlags, Common::WarningTypeSizeLessThanMinimum)) {
            descriptions.append(QLatin1String("Size is less than minimal"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeNoKeywords)) {
            descriptions.append(QLatin1String("Item has no keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTooFewKeywords)) {
            descriptions.append(QLatin1String("There's less than 7 keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTooManyKeywords)) {
            descriptions.append(QLatin1String("There's more than 50 keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeDescriptionIsEmpty)) {
            descriptions.append(QLatin1String("Description is empty"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeDescriptionTooBig)) {
            descriptions.append(QLatin1String("Description is too long"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeDescriptionNotEnoughWords)) {
            descriptions.append(QLatin1String("Description has too few words"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeDescriptionTooBig)) {
            descriptions.append(QLatin1String("Description is too long"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTitleIsEmpty)) {
            descriptions.append(QLatin1String("Title is empty"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTitleNotEnoughWords)) {
            descriptions.append(QLatin1String("Title has too few words"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTitleTooManyWords)) {
            descriptions.append(QLatin1String("Title has too many words"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeTitleTooBig)) {
            descriptions.append(QLatin1String("Title is too long"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeSpellErrorsInKeywords)) {
            descriptions.append(QLatin1String("Keywords have spelling error(s)"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeSpellErrorsInDescription)) {
            descriptions.append(QLatin1String("Description has spelling error(s)"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningTypeSpellErrorsInTitle)) {
            descriptions.append(QLatin1String("Title has spelling error(s)"));
        }
    }

    WarningsModel::WarningsModel(QObject *parent):
        QSortFilterProxyModel(parent),
        m_ShowOnlySelected(false)
    {
    }

    QStringList WarningsModel::describeWarnings(int index) const {
        QStringList descriptions;

        if (0 <= index && index < rowCount()) {
            QAbstractItemModel *sourceItemModel = sourceModel();
            Models::ArtItemsModel *artItemsModel = dynamic_cast<Models::ArtItemsModel *>(sourceItemModel);
            QModelIndex originalIndex = mapToSource(this->index(index, 0));
            int row = originalIndex.row();
            Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(row);

            int warningsFlags = metadata->getWarningsFlags();
            describeWarningFlags(warningsFlags, descriptions);
        }

        return descriptions;
    }

    void WarningsModel::update() {
        invalidateFilter();
        emit warningsCountChanged();
    }

    int WarningsModel::getOriginalIndex(int index) const {
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    void WarningsModel::sourceRowsRemoved(QModelIndex, int, int) {
        emit warningsCountChanged();
    }

    void WarningsModel::sourceRowsInserted(QModelIndex, int, int) {
        emit warningsCountChanged();
    }

    void WarningsModel::sourceModelReset() {
        emit warningsCountChanged();
    }

    bool WarningsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        QAbstractItemModel *sourceItemModel = sourceModel();
        Models::ArtItemsModel *artItemsModel = dynamic_cast<Models::ArtItemsModel *>(sourceItemModel);
        Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        int warningsFlags = metadata->getWarningsFlags();
        bool anyWarnings = warningsFlags != Common::WarningTypeNoWarnings;

        bool rowIsOk = anyWarnings;

        if (m_ShowOnlySelected) {
            rowIsOk = metadata->getIsSelected() && anyWarnings;
        }

        return rowIsOk;
    }

    void WarningsModel::setSourceModel(QAbstractItemModel *sourceModel) {
        QSortFilterProxyModel::setSourceModel(sourceModel);

        QObject::connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                         this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
        QObject::connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                         this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
        QObject::connect(sourceModel, SIGNAL(modelReset()),
                         this, SLOT(sourceModelReset()));
    }
}
