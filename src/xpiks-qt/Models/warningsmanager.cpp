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

#include "warningsmanager.h"
#include <QImageReader>
#include <QDebug>
#include "warningsinfo.h"
#include "../Helpers/globalimageprovider.h"
#include "../Helpers/appsettings.h"
#include "../Models/artiteminfo.h"
#include "../Commands/commandmanager.h"
#include "settingsmodel.h"

namespace Models {
    int WarningsManager::getWarningsCount() {
        int count = 0;

        foreach(WarningsInfo *info, m_WarningsBufferList) {
            if (info->hasWarnings()) count++;
        }

        return count;
    }

    void WarningsManager::checkForWarnings(const QList<ArtItemInfo *> &artworks) {
        beginResetModel();
        {
            qDeleteAll(m_WarningsBufferList);
            m_WarningsBufferList.clear();

            foreach (ArtItemInfo *itemInfo, artworks) {
                m_WarningsBufferList.append(new WarningsInfo(itemInfo));
            }

            recheckItems();
        }
        endResetModel();
    }

    void WarningsManager::recheckItems() {
        initConstraintsFromSettings();

        beginResetModel();
        m_WarningsList.clear();

        foreach(WarningsInfo *info, m_WarningsBufferList) {
            info->clearWarnings();

            if (checkItem(info)) {
                m_WarningsList.append(info);
            }
        }

        endResetModel();

        emit warningsCountChanged();
    }

    void WarningsManager::recheckItem(int itemIndex) {
        if (itemIndex < 0 || itemIndex >= m_WarningsList.length()) {
            return;
        }

        WarningsInfo *info = m_WarningsList[itemIndex];
        info->clearWarnings();

        if (!checkItem(info)) {
            beginRemoveRows(QModelIndex(), itemIndex, itemIndex);
            m_WarningsList.removeAt(itemIndex);
            endRemoveRows();
        } else {
            emit dataChanged(this->index(itemIndex), this->index(itemIndex));
        }
    }

    bool WarningsManager::checkItem(WarningsInfo *wi) {
        ArtworkMetadata *metadata = wi->getArtworkMetadata();

        bool hasWarnings = false;

        hasWarnings = checkDimensions(wi, metadata) || hasWarnings;
        hasWarnings = checkKeywordsCount(wi, metadata) || hasWarnings;
        hasWarnings = checkDescriptionLength(wi, metadata) || hasWarnings;
        hasWarnings = checkTitleWordsCount(wi, metadata) || hasWarnings;

        return hasWarnings;
    }

    bool WarningsManager::checkDimensions(WarningsInfo *wi, ArtworkMetadata *am) const {
        bool hasWarnings = false;

        const QString &filePath = am->getFilepath();
        QSize size;
        Q_ASSERT(m_ImageProvider != NULL);
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
            hasWarnings = true;
        }

        return hasWarnings;
    }

    bool WarningsManager::checkKeywordsCount(WarningsInfo *wi, ArtworkMetadata *am) const {
        bool hasWarnings = false;
        int keywordsCount = am->getKeywordsCount();

        if (keywordsCount == 0) {
            wi->addWarning("Image has no keywords");
            hasWarnings = true;
        }

        if (keywordsCount > m_MaximumKeywordsCount) {
            QString warning = QString("Keywords count (%1) exceeds maximum (%2)")
                    .arg(keywordsCount)
                    .arg(m_MaximumKeywordsCount);
            wi->addWarning(warning);
            hasWarnings = true;
        }

        return hasWarnings;
    }

    bool WarningsManager::checkDescriptionLength(WarningsInfo *wi, ArtworkMetadata *am) const {
        bool hasWarnings = false;
        const QString &description = am->getDescription();

        int descriptionLength = description.simplified().length();
        if (descriptionLength == 0) {
            wi->addWarning("Description is empty");
            hasWarnings = true;
        } else {
            if (descriptionLength > m_MaximumDescriptionLength) {
                QString warning = QString("Description length (%1) exceeds maximum (%2)")
                        .arg(descriptionLength)
                        .arg(m_MaximumDescriptionLength);
                wi->addWarning(warning);
                hasWarnings = true;
            }

            if (description.split(QChar::Space, QString::SkipEmptyParts).length() < 3) {
                QString warning = QString("Description should contain at least three words");
                wi->addWarning(warning);
                hasWarnings = true;
            }
        }

        return hasWarnings;
    }

    bool WarningsManager::checkTitleWordsCount(WarningsInfo *wi, ArtworkMetadata *am) const {
        bool hasWarnings = false;

        const QString &title = am->getTitle();

        if (!title.simplified().isEmpty()) {
            QStringList parts = title.split(QChar::Space, QString::SkipEmptyParts);
            int partsLength = parts.length();

            if (partsLength < 3) {
                QString warning = QString("Title should contain at least three words");
                wi->addWarning(warning);
                hasWarnings = true;
            }

            if (partsLength > 10) {
                QString warning = QString("Title should not contain more than ten words");
                wi->addWarning(warning);
                hasWarnings = true;
            }
        } else {
            wi->addWarning("Title is empty and will be copied from description");
            hasWarnings = true;
        }

        return hasWarnings;
    }

    void WarningsManager::initConstraintsFromSettings() {
        SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        m_MaximumDescriptionLength = settingsModel->getMaxDescriptionLength();
        m_MinimumMegapixels = settingsModel->getMinMegapixelCount();
        m_MaximumKeywordsCount = settingsModel->getMaxKeywordsCount();
    }

    int WarningsManager::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_WarningsList.count();
    }

    QVariant WarningsManager::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_WarningsList.count())
            return QVariant();

        WarningsInfo *warningsInfo = m_WarningsList.at(index.row());

        switch (role) {
        case ImagePathRole:
            return warningsInfo->getFilePath();
        case WarningsListRole:
            return warningsInfo->getWarnings();
        case ItemIndexRole:
            return warningsInfo->getIndex();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> WarningsManager::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ImagePathRole] = "filename";
        roles[WarningsListRole] = "warnings";
        roles[ItemIndexRole] = "itemindex";
        return roles;
    }
}
