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
#include <QObject>
#include <QStringList>
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"
#include "../Models/imageartwork.h"
#include "../Common/basickeywordsmodel.h"
#include "warningssettingsmodel.h"

namespace Warnings {
    void describeWarningFlags(Common::WarningFlags warningsFlags,
                              Models::ArtworkMetadata *metadata,
                              const WarningsSettingsModel *settingsModel,
                              QStringList &descriptions) {

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::SizeLessThanMinimum)) {
            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
#ifdef QT_DEBUG
            Q_ASSERT(image != NULL);
            {
#else
            if (image != NULL) {
#endif
                QSize size = image->getImageSize();
                int x = size.width();
                int y = size.height();
                descriptions.append(QObject::tr("Image size %1 x %2 is less than minimal").arg(x).arg(y));
            }
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::NoKeywords)) {
            descriptions.append(QObject::tr("Item has no keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TooFewKeywords)) {
            int minKeywordsCount = settingsModel->getMinKeywordsCount();
            descriptions.append(QObject::tr("There's less than %1 keywords").arg(minKeywordsCount));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TooManyKeywords)) {
            Common::BasicKeywordsModel *keywordsModel = metadata->getKeywordsModel();
            descriptions.append(QObject::tr("There are too many keywords (%1)").arg(keywordsModel->getKeywordsCount()));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::DescriptionIsEmpty)) {
            descriptions.append(QObject::tr("Description is empty"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::DescriptionNotEnoughWords)) {
            int minWordsCount = settingsModel->getMinWordsCount();
            descriptions.append(QObject::tr("Description should not have less than %1 words").arg(minWordsCount));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::DescriptionTooBig)) {
            descriptions.append(QObject::tr("Description is too long (%1 symbols)").arg(metadata->getDescription().length()));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TitleIsEmpty)) {
            descriptions.append(QObject::tr("Title is empty"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TitleNotEnoughWords)) {
            int minWordsCount = settingsModel->getMinWordsCount();
            descriptions.append(QObject::tr("Title should not have less than %1 words").arg(minWordsCount));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TitleTooManyWords)) {
            descriptions.append(QObject::tr("Title has too many words"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::TitleTooBig)) {
            descriptions.append(QObject::tr("Title is too long (%1 symbols)").arg(metadata->getTitle().length()));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::SpellErrorsInKeywords)) {
            descriptions.append(QObject::tr("Keywords have spelling error(s)"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::SpellErrorsInDescription)) {
            descriptions.append(QObject::tr("Description has spelling error(s)"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::SpellErrorsInTitle)) {
            descriptions.append(QObject::tr("Title has spelling error(s)"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::FileIsTooBig)) {
            double filesizeMB = settingsModel->getMaxFilesizeMB();
            QString formattedSize = QString::number(filesizeMB, 'f', 1);
            descriptions.append(QObject::tr("File size is larger than %1 MB").arg(formattedSize));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::KeywordsInDescription)) {
            descriptions.append(QObject::tr("Description contains some of the keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::KeywordsInTitle)) {
            descriptions.append(QObject::tr("Title contains some of the keywords"));
        }

        if (Common::HasFlag(warningsFlags, Common::WarningFlags::FilenameSymbols)) {
            descriptions.append(QObject::tr("Filename contains special characters or spaces"));
        }
    }

    WarningsModel::WarningsModel(QObject *parent):
        QSortFilterProxyModel(parent),
        m_ShowOnlySelected(false),
        m_WarningsSettingsModel(nullptr)
    {
    }

    QStringList WarningsModel::describeWarnings(int index) const {
        Q_ASSERT(m_WarningsSettingsModel != nullptr);
        QStringList descriptions;

        if (0 <= index && index < rowCount()) {
            QAbstractItemModel *sourceItemModel = sourceModel();
            Models::ArtItemsModel *artItemsModel = dynamic_cast<Models::ArtItemsModel *>(sourceItemModel);
            Q_ASSERT(artItemsModel != NULL);
            QModelIndex originalIndex = mapToSource(this->index(index, 0));
            int row = originalIndex.row();
            Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(row);

            if (metadata != NULL) {
                Common::WarningFlags warningsFlags = metadata->getWarningsFlags();
                describeWarningFlags(warningsFlags, metadata, m_WarningsSettingsModel, descriptions);
            }
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
        Q_ASSERT(artItemsModel != NULL);
        Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(sourceRow);

        bool rowIsOk = false;

        if (metadata != NULL) {
            Common::WarningFlags warningsFlags = metadata->getWarningsFlags();
            bool anyWarnings = warningsFlags != Common::WarningFlags::None;

            rowIsOk = anyWarnings;

            if (m_ShowOnlySelected) {
                rowIsOk = metadata->isSelected() && anyWarnings;
            }
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
