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
#include "../Helpers/appsettings.h"

namespace Models {
    void WarningsManager::checkForWarnings(const QList<ArtworkMetadata *> &artworks)
    {
        initConstraintsFromSettings();

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

        double currentProd = size.width() * size.height() / 1000000.0;
        if (currentProd < m_MinimumMegapixels) {
            QString warning = QString("Image size (%1 Megapixels) is less than minimum (%2 Megapixels)")
                    .arg(currentProd, 2)
                    .arg(m_MinimumMegapixels);
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

    void WarningsManager::initConstraintsFromSettings()
    {
        Helpers::AppSettings settings;
        m_MaximumDescriptionLength = settings.value(Constants::MAX_DESCRIPTION_LENGTH, 200).toInt();
        m_MinimumMegapixels = settings.value(Constants::MIN_MEGAPIXEL_COUNT, 4.0).toDouble();
        m_MaximumKeywordsCount = settings.value(Constants::MAX_KEYWORD_COUNT, 50).toInt();
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
