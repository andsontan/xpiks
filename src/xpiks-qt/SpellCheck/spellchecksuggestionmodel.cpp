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

#include "spellchecksuggestionmodel.h"
#include <QQmlEngine>

#include "keywordspellsuggestions.h"
#include "../Models/artworkmetadata.h"
#include "spellcheckerservice.h"

namespace SpellCheck {
    SpellCheckSuggestionModel::SpellCheckSuggestionModel():
        QAbstractListModel()
    {
    }

    QObject *SpellCheckSuggestionModel::getSuggestionItself(int index) const {
        KeywordSpellSuggestions *item = NULL;

        if (index >= 0 && index < m_KeywordsSuggestions.length()) {
            item = m_KeywordsSuggestions[index];
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    void SpellCheckSuggestionModel::clearModel() {
        beginResetModel();
        qDeleteAll(m_KeywordsSuggestions);
        m_KeywordsSuggestions.clear();
        endResetModel();
    }

    void SpellCheckSuggestionModel::submitCorrections() const {
        foreach (KeywordSpellSuggestions *item, m_KeywordsSuggestions) {
            int originalIndex = item->getOriginalIndex();
            const QString &keyword = item->getKeyword();
            const QString &replacement = item->getReplacement();
            m_CurrentMetadata->replaceKeyword(originalIndex, keyword, replacement);
        }
    }

    void SpellCheckSuggestionModel::setupModel(SpellCheckerService *service, Models::ArtworkMetadata *metadata) {
        Q_ASSERT(service != NULL);
        Q_ASSERT(metadata != NULL);

        QList<KeywordSpellSuggestions*> suggestionsRequests = metadata->createSuggestionsRequests();
        int length = suggestionsRequests.length();

        for (int i = 0; i < length; ++i) {
            KeywordSpellSuggestions *item = suggestionsRequests[i];
            const QString &keyword = item->getKeyword();
            QStringList suggestions = service->suggestCorrections(keyword);
            item->setSuggestions(suggestions);
        }

        m_CurrentMetadata = metadata;

        beginResetModel();
        qDeleteAll(m_KeywordsSuggestions);
        m_KeywordsSuggestions.clear();
        m_KeywordsSuggestions.append(suggestionsRequests);
        endResetModel();
    }

    int SpellCheckSuggestionModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsSuggestions.length();
    }

    QVariant SpellCheckSuggestionModel::data(const QModelIndex &index, int role) const {
        Q_UNUSED(index);
        Q_UNUSED(role);
        return QVariant();
    }
}
