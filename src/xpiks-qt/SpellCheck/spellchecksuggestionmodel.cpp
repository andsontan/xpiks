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
#include "ispellcheckable.h"
#include "../Commands/commandmanager.h"

namespace SpellCheck {
    SpellCheckSuggestionModel::SpellCheckSuggestionModel():
        QAbstractListModel(),
        Common::BaseEntity()
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
            if (item->getIsSelected()) {
                int originalIndex = item->getOriginalIndex();
                const QString &keyword = item->getWord();
                const QString &replacement = item->getReplacement();
                m_CurrentItem->replaceKeyword(originalIndex, keyword, replacement);
            }
        }
    }

    void SpellCheckSuggestionModel::setupModel(SpellCheckerService *service, SpellCheck::ISpellCheckable *item) {
        Q_ASSERT(service != NULL);
        Q_ASSERT(item != NULL);

        QList<KeywordSpellSuggestions*> suggestionsRequests = item->createSuggestionsList();
        setupSuggestions(suggestionsRequests);

        beginResetModel();
        m_CurrentItem = item;
        qDeleteAll(m_KeywordsSuggestions);
        m_KeywordsSuggestions.clear();
        m_KeywordsSuggestions.append(suggestionsRequests);
        endResetModel();
    }

    void SpellCheckSuggestionModel::setupSuggestions(const QList<KeywordSpellSuggestions*> &items) {
        SpellCheckerService *service = m_CommandManager->getSpellCheckerService();

        foreach (KeywordSpellSuggestions* item, items) {
            QStringList suggestions = service->suggestCorrections(item->getWord());
            item->setSuggestions(suggestions);
        }
    }

    int SpellCheckSuggestionModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsSuggestions.length();
    }

    QVariant SpellCheckSuggestionModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) { return QVariant(); }

        KeywordSpellSuggestions *item = m_KeywordsSuggestions[index.row()];

        switch (role) {
        case WordRole:
            return item->getWord();
        case ReplacementIndexRole:
            return item->getReplacementIndex();
        case IsSelectedRole:
            return item->getIsSelected();
        default:
            return QVariant();
        }
    }

    bool SpellCheckSuggestionModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        if (!index.isValid()) return false;
        int roleToUpdate = 0;

        switch (role) {
        case EditIsSelectedRole:
            m_KeywordsSuggestions[index.row()]->setIsSelected(value.toBool());
            roleToUpdate = IsSelectedRole;
            break;
        default:
            return false;
        }

        emit dataChanged(index, index, QVector<int>() << roleToUpdate);

        return true;
    }

    QHash<int, QByteArray> SpellCheckSuggestionModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[WordRole] = "word";
        roles[ReplacementIndexRole] = "replacementindex";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        return roles;
    }
}
