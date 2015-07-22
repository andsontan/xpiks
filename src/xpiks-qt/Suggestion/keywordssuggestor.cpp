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

#include <QHash>
#include <QString>
#include "keywordssuggestor.h"

namespace Suggestion {
    void KeywordsSuggestor::setSuggestedArtworks(const QList<SuggestionArtwork *> &suggestedArtworks) {
        beginResetModel();
        qDeleteAll(m_Suggestions);
        m_Suggestions.clear();
        m_Suggestions.append(suggestedArtworks);
        endResetModel();
        m_SelectedArtworksCount = 0;
        unsetInProgress();
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clear();
        m_AllOtherKeywords.clear();
    }

    void KeywordsSuggestor::clear() {
        beginResetModel();
        qDeleteAll(m_Suggestions);
        m_Suggestions.clear();
        endResetModel();
        m_SelectedArtworksCount = 0;
        unsetInProgress();
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clear();
        m_AllOtherKeywords.clear();
    }

    QString KeywordsSuggestor::removeSuggestedKeywordAt(int keywordIndex) {
        QString keyword;
        m_SuggestedKeywords.removeKeyword(keywordIndex, keyword);
        emit suggestedKeywordsCountChanged();
        return keyword;
    }

    QString KeywordsSuggestor::removeOtherKeywordAt(int keywordIndex) {
        QString keyword;
        m_AllOtherKeywords.removeKeyword(keywordIndex, keyword);
        emit otherKeywordsCountChanged();
        return keyword;
    }

    void KeywordsSuggestor::setArtworkSelected(int index, bool newState) {
        if (index < 0 || index >= m_Suggestions.length()) {
            return;
        }

        SuggestionArtwork *suggestionArtwork = m_Suggestions[index];
        suggestionArtwork->setIsSelected(newState);

        int sign = newState ? +1 : -1;
        accountKeywords(suggestionArtwork->getKeywords(), sign);
        m_SelectedArtworksCount += sign;

        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        updateSuggestedKeywords();
    }

    void KeywordsSuggestor::searchArtworks(const QString &searchTerm) {
        if (!m_IsInProgress) {
            setInProgress();
            m_QueryEngine.submitQuery(searchTerm.split(' '));
        }
    }

    int KeywordsSuggestor::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Suggestions.length();
    }

    QVariant KeywordsSuggestor::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_Suggestions.count())
            return QVariant();

        SuggestionArtwork *suggestionArtwork = m_Suggestions.at(index.row());

        switch (role) {
        case UrlRole:
            return suggestionArtwork->getUrl();
        case IsSelectedRole:
            return suggestionArtwork->getIsSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> KeywordsSuggestor::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[UrlRole] = "url";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void KeywordsSuggestor::accountKeywords(const QStringList &keywords, int sign) {
        foreach(const QString &keyword, keywords) {
            if (m_KeywordsHash.contains(keyword)) {
                m_KeywordsHash[keyword] += sign;
            } else {
                m_KeywordsHash.insert(keyword, 1);
            }
        }
    }

    QSet<QString> KeywordsSuggestor::getSelectedArtworksKeywords() const {
        QSet<QString> allKeywords;

        foreach (SuggestionArtwork *artwork, m_Suggestions) {
            if (artwork->getIsSelected()) {
                QSet<QString> currentKeywords = artwork->getKeywords().toSet();
                allKeywords.unite(currentKeywords);
            }
        }

        return allKeywords;
    }

    void KeywordsSuggestor::updateSuggestedKeywords() {
        QStringList keywords;

        QHash<QString, int>::const_iterator it = m_KeywordsHash.constBegin();
        QHash<QString, int>::const_iterator itEnd = m_KeywordsHash.constEnd();

        int threshold = 0;
        if (m_SelectedArtworksCount <= 2) {
            threshold = qMax(m_SelectedArtworksCount, 1);
        } else if (m_SelectedArtworksCount <= 4) {
            threshold = 2;
        } else if (m_SelectedArtworksCount <= 6) {
            threshold = 3;
        } else if (m_SelectedArtworksCount <= 8) {
            threshold = 4;
        } else {
            threshold = m_SelectedArtworksCount / 2 - 1;
        }

        for (; it != itEnd; ++it) {
            if (it.value() >= threshold) {
                keywords.append(it.key());
            }
        }

        m_SuggestedKeywords.reset(keywords);

        if (m_SelectedArtworksCount != 0) {
            QSet<QString> allKeywords = getSelectedArtworksKeywords();
            QSet<QString> suggestedKeywords = keywords.toSet();
            QSet<QString> otherKeywords = allKeywords.subtract(suggestedKeywords);

            m_AllOtherKeywords.reset(otherKeywords.toList());
        }
#ifndef QT_DEBUG
        else {
            m_AllOtherKeywords.clear();
            m_SuggestedKeywords.clear();
            m_KeywordsHash.clear();
        }
#endif

        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
    }
}
