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

#include <QHash>
#include <QString>
#include "keywordssuggestor.h"
#include "suggestionartwork.h"
#include "../Commands/commandmanager.h"

namespace Suggestion {
    void KeywordsSuggestor::setSuggestedArtworks(const QVector<SuggestionArtwork *> &suggestedArtworks) {
        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearModel();
        m_AllOtherKeywords.clearModel();
        beginResetModel();
        qDeleteAll(m_Suggestions);
        m_Suggestions.clear();
        m_Suggestions << suggestedArtworks;
        endResetModel();
        unsetInProgress();
        emit suggestionArrived();
    }

    void KeywordsSuggestor::clear() {
        m_SelectedArtworksCount = 0;
        m_KeywordsHash.clear();
        m_SuggestedKeywords.clearModel();
        m_AllOtherKeywords.clearModel();
        beginResetModel();
        qDeleteAll(m_Suggestions);
        m_Suggestions.clear();
        endResetModel();
        unsetInProgress();
    }

    QString KeywordsSuggestor::removeSuggestedKeywordAt(int keywordIndex) {
        QString keyword;
        m_SuggestedKeywords.takeKeywordAt(keywordIndex, keyword);
        emit suggestedKeywordsCountChanged();
        return keyword;
    }

    QString KeywordsSuggestor::removeOtherKeywordAt(int keywordIndex) {
        QString keyword;
        m_AllOtherKeywords.takeKeywordAt(keywordIndex, keyword);
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
        accountKeywords(suggestionArtwork->getKeywordsSet(), sign);
        m_SelectedArtworksCount += sign;

        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
        updateSuggestedKeywords();
    }

    void KeywordsSuggestor::searchArtworks(const QString &searchTerm) {
        if (!m_IsInProgress && !searchTerm.simplified().isEmpty()) {
            setInProgress();

            QStringList searchTerms = searchTerm.split(QChar::Space, QString::SkipEmptyParts);

            if (m_UseLocal) {
                m_QueryEngine.submitLocalQuery(m_LocalLibrary, searchTerms);
                m_CommandManager->reportUserAction(Conectivity::UserActionSuggestionLocal);
            } else {
                m_QueryEngine.submitQuery(searchTerms);
                m_CommandManager->reportUserAction(Conectivity::UserActionSuggestionRemote);
            }
        }
    }

    int KeywordsSuggestor::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Suggestions.length();
    }

    QVariant KeywordsSuggestor::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) { return QVariant(); }

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

    void KeywordsSuggestor::accountKeywords(const QSet<QString> &keywords, int sign) {
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
                const QSet<QString> &currentKeywords = artwork->getKeywordsSet();
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
        } else if (m_SelectedArtworksCount <= 5) {
            threshold = 2;
        } else if (m_SelectedArtworksCount <= 8) {
            threshold = 3;
        } else if (m_SelectedArtworksCount <= 10) {
            threshold = 4;
        } else {
            threshold = m_SelectedArtworksCount / 2 - 1;
        }

        for (; it != itEnd; ++it) {
            if (it.value() >= threshold) {
                keywords.append(it.key());
            }
        }

        m_SuggestedKeywords.resetKeywords(keywords);

        if (m_SelectedArtworksCount != 0) {
            QSet<QString> allKeywords = getSelectedArtworksKeywords();
            QSet<QString> suggestedKeywords = keywords.toSet();
            QSet<QString> otherKeywords = allKeywords.subtract(suggestedKeywords);

            m_AllOtherKeywords.resetKeywords(otherKeywords.toList());
        }
#ifndef QT_DEBUG
        else {
            m_AllOtherKeywords.clearKeywords();
            m_SuggestedKeywords.clearKeywords();
            m_KeywordsHash.clear();
        }
#endif

        emit suggestedKeywordsCountChanged();
        emit otherKeywordsCountChanged();
    }
}
