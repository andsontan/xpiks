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
#include <QDebug>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/keywordspellsuggestions.h"
#include "../SpellCheck/spellcheckiteminfo.h"

namespace Common {

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_KeywordsList.length()) {
            return QVariant();
        }

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
        m_SpellCheckResults.insert(0, n, true);

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
        bool indexValid = 0 <= index && index < m_KeywordsList.length();
        if (indexValid) {
            beginRemoveRows(QModelIndex(), index, index);
            keyword = m_KeywordsList.takeAt(index);
            m_SpellCheckResults.removeAt(index);
            endRemoveRows();
        }
        return indexValid;
    }

    bool BasicKeywordsModel::setDescription(const QString &value) {
        bool result = value != m_Description;
        if (result) { m_Description = value; }
        return result;
    }

    bool BasicKeywordsModel::setTitle(const QString &value) {
        bool result = value != m_Title;
        if (result) { m_Title = value; }
        return result;
    }

    void BasicKeywordsModel::updateDescriptionSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> descriptionErrors;
        QStringList descriptionWords = getDescriptionWords();
        foreach (const QString &word, descriptionWords) {
            if (results.value(word, true) == false) {
                descriptionErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setDescriptionErrors(descriptionErrors);
    }

    void BasicKeywordsModel::updateTitleSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> titleErrors;
        QStringList titleWords = getTitleWords();
        foreach (const QString &word, titleWords) {
            if (results.value(word, true) == false) {
                titleErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setTitleErrors(titleErrors);
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

    void BasicKeywordsModel::setSpellCheckResults(const QVector<SpellCheck::SpellCheckQueryItem *> &items) {
        int size = items.length();
        for (int i = 0; i < size; ++i) {
            SpellCheck::SpellCheckQueryItem *item = items.at(i);
            int index = item->m_Index;
            if (0 <= index && index <= m_KeywordsList.length()) {
                if (m_KeywordsList[index] == item->m_Word) {
                    m_SpellCheckResults[index] = item->m_IsCorrect;
                }
            }
        }

        int indexToUpdate = -1;

        if (items.length() == 1) {
            indexToUpdate = items.first()->m_Index;
        }

        emitSpellCheckChanged(indexToUpdate);
    }

    void BasicKeywordsModel::setSpellCheckResults(const QHash<QString, bool> &results) {
        updateDescriptionSpellErrors(results);
        updateTitleSpellErrors(results);

        emit spellCheckResultsReady();
    }

    QVector<SpellCheck::KeywordSpellSuggestions *> BasicKeywordsModel::createKeywordsSuggestionsList() {
        QVector<SpellCheck::KeywordSpellSuggestions *> spellCheckSuggestions;
        int length = m_KeywordsList.length();
        spellCheckSuggestions.reserve(length/2);

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

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(int)), this, SLOT(spellCheckRequestReady(int)));
    }

    QStringList BasicKeywordsModel::getDescriptionWords() const {
        QStringList words = m_Description.split(" ", QString::SkipEmptyParts);
        return words;
    }

    QStringList BasicKeywordsModel::getTitleWords() const {
        QStringList words = m_Title.split(" ", QString::SkipEmptyParts);
        return words;
    }

    void BasicKeywordsModel::spellCheckRequestReady(int index) {
        qDebug() << "SpellCheck results ready at index" << index;
        emitSpellCheckChanged(index);
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
