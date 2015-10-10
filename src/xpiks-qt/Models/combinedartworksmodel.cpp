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

#include "combinedartworksmodel.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/combinededitcommand.h"
#include "../Commands/commandmanager.h"
#include "../Commands/commandbase.h"
#include "../Suggestion/keywordssuggestor.h"
#include "artworkmetadata.h"
#include "artiteminfo.h"

namespace Models {
    CombinedArtworksModel::~CombinedArtworksModel() { qDeleteAll(m_ArtworksList); }

    void CombinedArtworksModel::initArtworks(const QList<ArtItemInfo *> &artworks)
    {
        int innerLength = m_ArtworksList.length();
        int start = innerLength == 0 ? 0 : innerLength - 1;
        int paramLength = artworks.length();
        if (paramLength > 0) {
            beginInsertRows(QModelIndex(), start, start + paramLength - 1);
            m_ArtworksList.append(artworks);
            endInsertRows();
        }
        m_IsModified = false;

        if (artworks.length() == 1) {
            enableAllFields();
        }
    }

    void CombinedArtworksModel::recombineArtworks() {
        if (m_ArtworksList.length() == 1) {
            assignFromOneArtwork();
        } else {
            assignFromManyArtworks();
        }
    }

    void CombinedArtworksModel::acceptSuggestedKeywords(const QStringList &keywords)  {
        foreach (const QString &keyword, keywords) {
            this->appendKeyword(keyword);
        }
    }

    void CombinedArtworksModel::setChangeDescription(bool value)  {
        if (Common::HasFlag(m_EditFlags, Common::EditDesctiption) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditDesctiption);
            emit changeDescriptionChanged();
        }
    }

    void CombinedArtworksModel::setChangeTitle(bool value) {
        if (Common::HasFlag(m_EditFlags, Common::EditTitle) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditTitle);
            emit changeTitleChanged();
        }
    }

    void CombinedArtworksModel::setChangeKeywords(bool value){
        if (Common::HasFlag(m_EditFlags, Common::EditKeywords) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::EditKeywords);
            emit changeKeywordsChanged();
        }
    }

    void CombinedArtworksModel::setAppendKeywords(bool value) {
        if (Common::HasFlag(m_EditFlags, Common::AppendKeywords) != value) {
            Common::ApplyFlag(m_EditFlags, value, Common::AppendKeywords);
            emit appendKeywordsChanged();
        }
    }

    void CombinedArtworksModel::resetModelData() {
        beginResetModel();
        qDeleteAll(m_ArtworksList);
        m_ArtworksList.clear();
        endResetModel();

        m_EditFlags = 0;
        // TEMPORARY (enable everything on initial launch) --
        Common::ApplyFlag(m_EditFlags, true, Common::EditTitle);
        Common::ApplyFlag(m_EditFlags, true, Common::EditDesctiption);
        Common::ApplyFlag(m_EditFlags, true, Common::EditKeywords);
        // TEMPORARY (enable everything on initial launch) --

        setDescription("");
        setTitle("");
        setKeywords(QStringList());
        m_CommonKeywordsSet.clear();
    }

    void CombinedArtworksModel::clearKeywords() {
        m_CommonKeywordsSet.clear();
        m_CommonKeywordsModel.clear();
    }

    void CombinedArtworksModel::processCombinedEditCommand() const {
        Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                    m_EditFlags,
                    m_ArtworksList,
                    m_ArtworkDescription,
                    m_ArtworkTitle,
                    m_CommonKeywordsModel.getKeywords());

        Commands::CommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
        Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
        m_CommandManager->updateArtworks(combinedResult->m_IndicesToUpdate);

        delete combinedResult;
    }

    void CombinedArtworksModel::enableAllFields(){
        setChangeDescription(true);
        setChangeTitle(true);
        setChangeKeywords(true);
    }

    void CombinedArtworksModel::assignFromOneArtwork() {
        Q_ASSERT(m_ArtworksList.length() == 1);
        ArtItemInfo *info = m_ArtworksList[0];
        ArtworkMetadata *metadata = info->getOrigin();

        initDescription(metadata->getDescription());
        initTitle(metadata->getTitle());

        if (!m_IsModified) {
            initKeywords(metadata->getKeywords());
        }
    }

    void CombinedArtworksModel::assignFromManyArtworks() {
        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        bool titleDiffer = false;
        QString description;
        QString title;
        QSet<QString> commonKeywords;

        int artworksCount = m_ArtworksList.length();
        for (int i = 0; i < artworksCount; ++i) {
            ArtItemInfo *info = m_ArtworksList[i];
            ArtworkMetadata *metadata = info->getOrigin();

            if (!anyItemsProcessed) {
                description = metadata->getDescription();
                title = metadata->getTitle();
                commonKeywords.unite(metadata->getKeywordsSet());
                anyItemsProcessed = true;
                continue;
            }

            const QString &currDescription = metadata->getDescription();
            const QString &currTitle = metadata->getTitle();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            titleDiffer = titleDiffer || title != currTitle;
            commonKeywords.intersect(metadata->getKeywordsSet());
        }

        if (artworksCount > 0) {
            if (descriptionsDiffer) {
                description = "";
            }

            if (titleDiffer) {
                title = "";
            }

            initDescription(description);
            initTitle(title);

            if (!m_IsModified) {
                initKeywords(commonKeywords.toList());
            }
        }
    }

    QString CombinedArtworksModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        if (m_CommonKeywordsModel.removeKeyword(keywordIndex, keyword)) {
            m_CommonKeywordsSet.remove(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }

        return keyword;
    }

    void CombinedArtworksModel::removeLastKeyword() {
        QString keyword;
        if (m_CommonKeywordsModel.removeLastKeyword(keyword)) {
            m_CommonKeywordsSet.remove(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &word) {
        QString keyword = word.simplified();
        if (!keyword.isEmpty() && !m_CommonKeywordsSet.contains(keyword)) {
            m_CommonKeywordsModel.appendKeyword(keyword);
            m_CommonKeywordsSet.insert(keyword);
            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::pasteKeywords(const QStringList &keywords) {
        if (!keywords.empty()) {
            foreach (const QString &keyword, keywords) {
                if (!m_CommonKeywordsSet.contains(keyword)) {
                    m_CommonKeywordsSet.insert(keyword);
                    m_CommonKeywordsModel.appendKeyword(keyword);
                }
            }

            emit keywordsCountChanged();
            m_IsModified = true;
        }
    }

    void CombinedArtworksModel::setArtworksSelected(int index, bool newState) {
        if (index < 0 || index >= m_ArtworksList.length()) {
            return;
        }

        m_ArtworksList[index]->setSelected(newState);
        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        emit selectedArtworksCountChanged();
    }

    void CombinedArtworksModel::removeSelectedArtworks() {
        int count = m_ArtworksList.length();
        QList<int> indicesToRemove;
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                indicesToRemove.append(i);
            }
        }

        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);

        recombineArtworks();
        emit artworksCountChanged();
    }

    void CombinedArtworksModel::saveEdits() const {
        processCombinedEditCommand();
    }

    int CombinedArtworksModel::getSelectedArtworksCount() const {
        int selectedCount = 0;
        int count = m_ArtworksList.length();
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    int CombinedArtworksModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworksList.count();
    }

    QVariant CombinedArtworksModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworksList.count())
            return QVariant();

        ArtItemInfo *artItemInfo = m_ArtworksList.at(index.row());

        switch (role) {
        case PathRole:
            return artItemInfo->getOrigin()->getFilepath();
        case IsSelectedRole:
            return artItemInfo->isSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> CombinedArtworksModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void CombinedArtworksModel::removeInnerItem(int row) {
        ArtItemInfo *info = m_ArtworksList[row];
        delete info;
        m_ArtworksList.removeAt(row);
    }
}
