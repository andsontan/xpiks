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

#include "deletekeywordsviewmodel.h"
#include <QTime>
#include "../Helpers/indiceshelper.h"
#include "metadataelement.h"
#include "../Commands/commandmanager.h"
#include "../Commands/deletekeywordscommand.h"
#include "../Common/defines.h"

namespace Models {
    DeleteKeywordsViewModel::DeleteKeywordsViewModel(QObject *parent):
        Common::AbstractListModel(parent),
        m_KeywordsToDeleteModel(m_HoldPlaceholder),
        m_CommonKeywordsModel(m_HoldPlaceholder)
    {
    }

    void DeleteKeywordsViewModel::setArtworks(std::vector<MetadataElement> &artworks) {
        size_t paramLength = artworks.size();

        if (paramLength > 0) {
            beginResetModel();
            m_ArtworksList = std::move(artworks);
            endResetModel();

            recombineKeywords();
        }
    }

    void DeleteKeywordsViewModel::removeUnavailableItems() {
        LOG_DEBUG << "#";
        QVector<int> indicesToRemove;
        size_t size = m_ArtworksList.size();
        for (size_t i = 0; i < size; i++) {
            MetadataElement &item = m_ArtworksList.at(i);

            if (item.getOrigin()->isUnavailable()) {
                indicesToRemove.append((int)i);
            }
        }

        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);

        removeItemsAtIndices(rangesToRemove);
        recombineKeywords();

        if (m_ArtworksList.empty()) {
            emit requestCloseWindow();
        }

        emit artworksCountChanged();
    }

    void DeleteKeywordsViewModel::removeKeywordToDeleteAt(int keywordIndex) {
        QString keyword;
        if (m_KeywordsToDeleteModel.takeKeywordAt(keywordIndex, keyword)) {
            emit keywordsToDeleteCountChanged();
        }
    }

    QString DeleteKeywordsViewModel::removeCommonKeywordAt(int keywordIndex) {
        QString keyword;
        if (m_CommonKeywordsModel.takeKeywordAt(keywordIndex, keyword)) {
            emit commonKeywordsCountChanged();
        }

        return keyword;
    }

    void DeleteKeywordsViewModel::appendKeywordToDelete(const QString &keyword) {
        if (m_KeywordsToDeleteModel.appendKeyword(keyword)) {
            emit keywordsToDeleteCountChanged();

            m_CommandManager->submitKeywordForSpellCheck(&m_KeywordsToDeleteModel, m_KeywordsToDeleteModel.rowCount() - 1);
        }
    }

    void DeleteKeywordsViewModel::pasteKeywordsToDelete(const QStringList &keywords) {
        LOG_DEBUG << keywords.length() << "keyword(s)";
        if (m_KeywordsToDeleteModel.appendKeywords(keywords) > 0) {
            emit keywordsToDeleteCountChanged();

            m_CommandManager->submitItemForSpellCheck(&m_KeywordsToDeleteModel, Common::SpellCheckKeywords);
        }
    }

    void DeleteKeywordsViewModel::removeSelectedArtworks() {
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

        if (!indicesToRemove.empty()) {
            LOG_INFO << "Removing" << indicesToRemove.size() << "item(s)";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);

            recombineKeywords();
            emit artworksCountChanged();
        }
    }

    void DeleteKeywordsViewModel::deleteKeywords() {
        LOG_INFO << "#";
        std::shared_ptr<Commands::DeleteKeywordsCommand> deleteKeywordsCommand(
                    new Commands::DeleteKeywordsCommand(m_ArtworksList, m_KeywordsToDeleteModel.getKeywords()));
        m_CommandManager->processCommand(deleteKeywordsCommand);
    }

    void DeleteKeywordsViewModel::resetModel() {
        LOG_DEBUG << "#";
        beginResetModel();
        m_ArtworksList.clear();
        endResetModel();

        m_CommonKeywordsModel.clearModel();
        m_KeywordsToDeleteModel.clearModel();
    }

    int DeleteKeywordsViewModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_ArtworksList.size();
    }

    QVariant DeleteKeywordsViewModel::data(const QModelIndex &index, int role) const {
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

    QHash<int, QByteArray> DeleteKeywordsViewModel::roleNames() const {
        QHash<int, QByteArray> names = QAbstractListModel::roleNames();
        names[FilepathRole] = "filepath";
        names[IsSelectedRole] = "isselected";
        return names;
    }

    void DeleteKeywordsViewModel::recombineKeywords() {
        LOG_DEBUG << "#";
        QHash<QString, int> keywordsHash;
        fillKeywordsHash(keywordsHash);

        QMultiMap<int, QString> selectedKeywords;

        auto hashIt = keywordsHash.constBegin();
        auto hashItEnd = keywordsHash.constEnd();

        for (; hashIt != hashItEnd; ++hashIt) {
            selectedKeywords.insert(hashIt.value(), hashIt.key());
        }

        auto it = selectedKeywords.constEnd();
        auto itBegin = selectedKeywords.constBegin();

        QStringList commonKeywords;
        commonKeywords.reserve(50);

        qsrand(QTime::currentTime().msec());
        int maxSize = 40 + qrand()%10;

        while (it != itBegin) {
            --it;

            int frequency = it.key();
            if (frequency == 0) { continue; }

            const QString &frequentKeyword = it.value();

            commonKeywords.append(frequentKeyword);
            if (commonKeywords.size() > maxSize) { break; }
        }

        m_CommonKeywordsModel.setKeywords(commonKeywords);
        emit commonKeywordsCountChanged();
    }

    void DeleteKeywordsViewModel::fillKeywordsHash(QHash<QString, int> &keywordsHash) {
        for (auto &item: m_ArtworksList) {
            auto *origin = item.getOrigin();
            const auto &keywordsSet = origin->getKeywordsSet();

            for (auto &keyword: keywordsSet) {
                if (keywordsHash.contains(keyword)) {
                    keywordsHash[keyword]++;
                } else {
                    keywordsHash.insert(keyword, 1);
                }
            }
        }
    }
}
