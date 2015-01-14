/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#include "warningsmanager.h"
#include <QImageReader>

namespace Models {
    void WarningsManager::checkForWarnings(const QList<ArtworkMetadata *> &artworks)
    {
        beginResetModel();

        qDeleteAll(m_WarningsList);
        m_WarningsList.clear();

        foreach (ArtworkMetadata *metadata, artworks) {
            checkItem(metadata);
        }

        endResetModel();

        emit warningsCountChanged();
    }

    void WarningsManager::checkItem(ArtworkMetadata *metadata)
    {
        const QString &filePath = metadata->getFilepath();
        WarningsInfo *wi = new WarningsInfo(filePath);

        checkDimensions(wi, metadata);
        checkKeywordsCount(wi, metadata);
        checkDescriptionLength(wi, metadata);

        m_WarningsList.append(wi);
    }

    void WarningsManager::checkDimensions(WarningsInfo *wi, ArtworkMetadata *am) const
    {
        const QString &filePath = am->getFilepath();
        QSize size;
        if (!m_ImageProvider->tryGetOriginalSize(filePath, size)) {
            QImageReader reader(filePath);
            size = reader.size();
        }

        qlonglong currentProd = size.width() * size.height();
        qlonglong minimalProd = m_MinimalSize.width() * m_MinimalSize.height();
        if (currentProd < minimalProd) {
            QString warning = QString("Image size (%1 x %2) is less than minimum (%3 x %4)")
                    .arg(size.width())
                    .arg(size.height())
                    .arg(m_MinimalSize.width())
                    .arg(m_MinimalSize.height());
            wi->addWarning(warning);
        }
    }

    void WarningsManager::checkKeywordsCount(WarningsInfo *wi, ArtworkMetadata *am) const
    {
        int keywordsCount = am->getKeywordsCount();

        if (keywordsCount == 0) {
            wi->addWarning("Image has no keywords");
        }

        if (keywordsCount > m_MaximumKeywordsCount) {
            QString warning = QString("Keywords count (%1) exceeds maximum (%2)")
                    .arg(keywordsCount)
                    .arg(m_MaximumKeywordsCount);
            wi->addWarning(warning);
        }
    }

    void WarningsManager::checkDescriptionLength(WarningsInfo *wi, ArtworkMetadata *am) const
    {
        int descriptionLength = am->getDescription().length();
        if (descriptionLength == 0) {
            wi->addWarning("Description is empty");
        }

        if (descriptionLength > m_MaximumDescriptionLength) {
            QString warning = QString("Description length (%1) exceeds maximum (%2)")
                    .arg(descriptionLength)
                    .arg(m_MaximumDescriptionLength);
            wi->addWarning(warning);
        }
    }

    int WarningsManager::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_WarningsList.count();
    }

    QVariant WarningsManager::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_WarningsList.count())
            return QVariant();

        WarningsInfo *warningsInfo = m_WarningsList.at(index.row());

        switch (role) {
        case ImagePathRole:
            return warningsInfo->getFilePath();
        case WarningsListRole:
            return warningsInfo->getWarnings();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> WarningsManager::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[ImagePathRole] = "filename";
        roles[WarningsListRole] = "warnings";
        return roles;
    }
}
