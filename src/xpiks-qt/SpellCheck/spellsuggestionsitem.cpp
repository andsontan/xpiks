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

#include "spellsuggestionsitem.h"
#include "ispellcheckable.h"

namespace SpellCheck {
    SpellSuggestionsItem::SpellSuggestionsItem(const QString &word, const QString &origin) :
        QAbstractListModel(),
        m_Word(word),
        m_ReplacementOrigin(origin),
        m_ReplacementIndex(0),
        m_IsSelected(true)
    {
    }

    SpellSuggestionsItem::SpellSuggestionsItem(const QString &word):
        QAbstractListModel(),
        m_Word(word),
        m_ReplacementOrigin((word)),
        m_ReplacementIndex(0),
        m_IsSelected(true)
    {
    }

    bool SpellSuggestionsItem::setReplacementIndex(int value) {
        bool result = value != m_ReplacementIndex;

        if (result) {
            QModelIndex prev = this->index(m_ReplacementIndex);
            QModelIndex curr = this->index(value);
            m_ReplacementIndex = value;
            QVector<int> roles;
            roles << IsSelectedRole;
            emit dataChanged(prev, prev, roles);
            emit dataChanged(curr, curr, roles);
        }

        return result;
    }

    void SpellSuggestionsItem::setSuggestions(const QStringList &suggestions) {
        beginResetModel();
        m_Suggestions.clear();
        m_Suggestions.append(suggestions);
        m_ReplacementIndex = 0;
        endResetModel();
    }

    int SpellSuggestionsItem::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_Suggestions.length();
    }

    QVariant SpellSuggestionsItem::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) return QVariant();

        int row = index.row();

        switch (role) {
        case SuggestionRole:
            return m_Suggestions.at(row);
        case IsSelectedRole:
            return (m_ReplacementIndex == row);
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags SpellSuggestionsItem::flags(const QModelIndex &index) const {
        int row = index.row();
        if (row < 0 || row >= m_Suggestions.length()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool SpellSuggestionsItem::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();
        if (row < 0 || row >= m_Suggestions.length()) return false;

        bool result = false;

        switch (role) {
        case EditReplacementIndexRole:
            result = setReplacementIndex(value.toInt());
            break;
        default:
            return false;
        }

        return result;
    }

    QHash<int, QByteArray> SpellSuggestionsItem::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[SuggestionRole] = "suggestion";
        roles[IsSelectedRole] = "isselected";
        roles[EditReplacementIndexRole] = "editreplacementindex";
        return roles;
    }

    KeywordSpellSuggestions::KeywordSpellSuggestions(const QString &keyword, int originalIndex, const QString &origin) :
        SpellSuggestionsItem(keyword, origin),
        m_OriginalIndex(originalIndex)
    {
    }

    KeywordSpellSuggestions::KeywordSpellSuggestions(const QString &keyword, int originalIndex):
        SpellSuggestionsItem(keyword),
        m_OriginalIndex(originalIndex)
    {
    }

    void KeywordSpellSuggestions::replaceToSuggested(ISpellCheckable *item) {
        const QString &word = getWord();
        const QString &replacement = getReplacement();
        item->replaceKeyword(m_OriginalIndex, word, replacement);
    }

    DescriptionSpellSuggestions::DescriptionSpellSuggestions(const QString &word):
        SpellSuggestionsItem(word)
    {
    }

    void DescriptionSpellSuggestions::replaceToSuggested(ISpellCheckable *item) {
        item->replaceWordInDescription(getWord(), getReplacement());
    }

    TitleSpellSuggestions::TitleSpellSuggestions(const QString &word):
        SpellSuggestionsItem(word)
    {
    }

    void TitleSpellSuggestions::replaceToSuggested(ISpellCheckable *item) {
        item->replaceWordInTitle(getWord(), getReplacement());
    }
}
