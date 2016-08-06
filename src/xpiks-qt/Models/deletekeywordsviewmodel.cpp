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
        Models::ArtworksViewModel(parent),
        m_KeywordsToDeleteModel(m_HoldPlaceholder),
        m_CommonKeywordsModel(m_HoldPlaceholder),
        m_CaseSensitive(false)
    {
    }

    void DeleteKeywordsViewModel::setArtworks(std::vector<MetadataElement> &artworks) {
        ArtworksViewModel::setArtworks(artworks);
        recombineKeywords();
    }

    bool DeleteKeywordsViewModel::removeUnavailableItems() {
        bool anyRemoved = ArtworksViewModel::removeUnavailableItems();

        if (anyRemoved) {
            if (!isEmpty()) {
                recombineKeywords();
            }
        }

        return anyRemoved;
    }

    bool DeleteKeywordsViewModel::doRemoveSelectedArtworks() {
        bool anyRemoved = ArtworksViewModel::doRemoveSelectedArtworks();

        if (anyRemoved) {
            if (!isEmpty()) {
                recombineKeywords();
            }
        }

        return anyRemoved;
    }

    void DeleteKeywordsViewModel::doResetModel() {
        ArtworksViewModel::doResetModel();

        m_CommonKeywordsModel.clearModel();
        m_KeywordsToDeleteModel.clearModel();

        setCaseSensitive(false);
    }

    void DeleteKeywordsViewModel::removeKeywordToDeleteAt(int keywordIndex) {
        LOG_DEBUG << "#";
        QString keyword;
        if (m_KeywordsToDeleteModel.takeKeywordAt(keywordIndex, keyword)) {
            emit keywordsToDeleteCountChanged();
        }
    }

    void DeleteKeywordsViewModel::removeLastKeywordToDelete() {
        LOG_DEBUG << "#";
        removeKeywordToDeleteAt(getKeywordsToDeleteCount() - 1);
    }

    void DeleteKeywordsViewModel::clearKeywordsToDelete() {
        LOG_DEBUG << "#";
        m_KeywordsToDeleteModel.clearKeywords();
    }

    QString DeleteKeywordsViewModel::removeCommonKeywordAt(int keywordIndex) {
        LOG_DEBUG << "index:" << keywordIndex;
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
        LOG_INFO << keywords.length() << "keyword(s)" << "|" << keywords;
        if (m_KeywordsToDeleteModel.appendKeywords(keywords) > 0) {
            emit keywordsToDeleteCountChanged();

            m_CommandManager->submitItemForSpellCheck(&m_KeywordsToDeleteModel, Common::SpellCheckKeywords);
        }
    }

    void DeleteKeywordsViewModel::deleteKeywords() {
        LOG_INFO << "keywords to delete:" << m_KeywordsToDeleteModel.getKeywordsCount();

        if (m_KeywordsToDeleteModel.getKeywordsCount() == 0) { return; }

        auto &artworksList = getArtworksList();
        auto keywordsList = m_KeywordsToDeleteModel.getKeywords();

        if (!m_CaseSensitive) {
            for (auto &keyword: keywordsList) {
                keyword = keyword.toLower();
            }
        }

        auto keywordsSet = keywordsList.toSet();

        std::shared_ptr<Commands::DeleteKeywordsCommand> deleteKeywordsCommand(
                    new Commands::DeleteKeywordsCommand(artworksList, keywordsSet, m_CaseSensitive));
        m_CommandManager->processCommand(deleteKeywordsCommand);
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

        LOG_INFO << "Found" << commonKeywords.size() << "common keywords";
        m_CommonKeywordsModel.setKeywords(commonKeywords);
        emit commonKeywordsCountChanged();
    }

    void DeleteKeywordsViewModel::fillKeywordsHash(QHash<QString, int> &keywordsHash) {
        processArtworks([](const MetadataElement&) { return true; },
        [&keywordsHash](int, ArtworkMetadata *metadata) {
            const auto &keywords = metadata->getKeywords();

            for (auto &keyword: keywords) {
                if (keywordsHash.contains(keyword)) {
                    keywordsHash[keyword]++;
                } else {
                    keywordsHash.insert(keyword, 1);
                }
            }
        });
    }
}
