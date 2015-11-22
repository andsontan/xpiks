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

#include "basickeywordsmodel.h"
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/keywordspellsuggestions.h"

namespace Common {

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_KeywordsList.count()) return QVariant();

        switch (role) {
        case KeywordRole:
            return m_KeywordsList.at(index.row());
        case IsCorrectRole:
            return m_SpellCheckResults.at(index.row());
        default:
            return QVariant();
        }
    }

    void BasicKeywordsModel::reset(const QStringList &items) {
        m_SpellCheckResults.clear();
        int n = items.length();
        while (n--) { m_SpellCheckResults.append(true); }

        beginResetModel();
        m_KeywordsList.clear();
        m_KeywordsList.append(items);
        endResetModel();
    }

    void BasicKeywordsModel::clear() {
        beginResetModel();
        m_KeywordsList.clear();
        m_SpellCheckResults.clear();
        endResetModel();
    }

    void BasicKeywordsModel::appendKeyword(const QString &keyword) {
        m_SpellCheckResults.append(true);
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_KeywordsList.append(keyword);
        endInsertRows();
    }

    bool BasicKeywordsModel::removeKeyword(int index, QString &keyword) {
        bool indexValid = index >= 0 && index < m_KeywordsList.length();
        if (indexValid) {
            beginRemoveRows(QModelIndex(), index, index);
            keyword = m_KeywordsList.takeAt(index);
            m_SpellCheckResults.removeAt(index);
            endRemoveRows();
        }
        return indexValid;
    }

    QString BasicKeywordsModel::retrieveKeyword(int wordIndex) {
        QString keyword;
        if (0 <= wordIndex && wordIndex < m_KeywordsList.length()) {
            keyword = m_KeywordsList.at(wordIndex);
        }

        return keyword;
    }

    QStringList BasicKeywordsModel::getKeywords() {
        return m_KeywordsList;
    }

    void BasicKeywordsModel::setSpellCheckResults(const QList<SpellCheck::SpellCheckQueryItem *> &items) {
        foreach (SpellCheck::SpellCheckQueryItem *item, items) {
            int index = item->m_Index;
            if (0 <= index && index <= m_KeywordsList.length()) {
                if (m_KeywordsList[index] == item->m_Word) {
                    m_SpellCheckResults[index] = item->m_IsCorrect;
                }
            }
        }

        int index = -1;

        if (items.length() == 1) {
            index = items.first()->m_Index;
        }

        emitSpellCheckChanged(index);
    }

    QList<SpellCheck::KeywordSpellSuggestions *> BasicKeywordsModel::createSuggestionsList() {
        QList<SpellCheck::KeywordSpellSuggestions *> spellCheckSuggestions;

        int length = m_KeywordsList.length();
        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                const QString &keyword = m_KeywordsList[i];
                SpellCheck::KeywordSpellSuggestions *suggestionsItem = new SpellCheck::KeywordSpellSuggestions(keyword, i);
                spellCheckSuggestions.append(suggestionsItem);
            }
        }

        return spellCheckSuggestions;
    }

    void BasicKeywordsModel::replaceKeyword(int index, const QString &existing, const QString &replacement) {
        if (0 <= index && index < m_KeywordsList.length()) {
            const QString &internal = m_KeywordsList.at(index);
            if (internal == existing) {
                m_KeywordsList[index] = replacement;
                m_SpellCheckResults[index] = true;
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole << KeywordRole);
            }
        }
    }

    void BasicKeywordsModel::emitSpellCheckChanged(int index) {
        int count = m_KeywordsList.length();

        if (index == -1) {
            if (count > 0) {
                QModelIndex start = this->index(0);
                QModelIndex end = this->index(count - 1);
                emit dataChanged(start, end, QVector<int>() << IsCorrectRole);
            }
        } else {
            if (0 <= index && index < count) {
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole);
            }
        }
    }

    QHash<int, QByteArray> BasicKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[IsCorrectRole] = "iscorrect";
        return roles;
    }
}
