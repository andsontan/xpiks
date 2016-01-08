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
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordvalidator.h"
#include "../Helpers/stringhelper.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(QObject *parent):
        QAbstractListModel(parent),
        m_SpellCheckInfo(NULL),
        m_WarningsFlags(0)
    {
    }

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

    bool BasicKeywordsModel::appendKeyword(const QString &keyword) {
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified();

        if (canBeAdded(sanitizedKeyword)) {
            int keywordsCount = m_KeywordsList.length();

            m_KeywordsSet.insert(sanitizedKeyword.toLower());
            m_SpellCheckResults.append(true);

            beginInsertRows(QModelIndex(), keywordsCount, keywordsCount);
            m_KeywordsList.append(sanitizedKeyword);
            endInsertRows();
            added = true;
        }

        return added;
    }

    bool BasicKeywordsModel::takeKeywordAt(int index, QString &removedKeyword) {
        bool removed = false;

        if (0 <= index && index < m_KeywordsList.length()) {
            const QString &keyword = m_KeywordsList.at(index);
            QString invariant = keyword.toLower();
            m_KeywordsSet.remove(invariant);

            beginRemoveRows(QModelIndex(), index, index);
            removedKeyword = m_KeywordsList.takeAt(index);
            bool wasCorrect = m_SpellCheckResults.takeAt(index);
            endRemoveRows();

            if (!wasCorrect) {
                emit spellCheckErrorsChanged();
            }

            removed = true;
        }

        return removed;
    }

    void BasicKeywordsModel::setKeywords(const QStringList &keywordsList) {
        clearKeywords();
        appendKeywords(keywordsList);
    }

    int BasicKeywordsModel::appendKeywords(const QStringList &keywordsList) {
        QStringList keywordsToAdd;
        int appendedCount = 0, size = keywordsList.length();
        keywordsToAdd.reserve(size);
        QSet<QString> accountedKeywords;

        for (int i = 0; i < size; ++i) {
            const QString &keyword = keywordsList.at(i);
            const QString &sanitizedKeyword = keyword.simplified();
            const QString &lowerCased = sanitizedKeyword.toLower();

            if (canBeAdded(sanitizedKeyword) && !accountedKeywords.contains(lowerCased)) {
                keywordsToAdd.append(sanitizedKeyword);
                accountedKeywords.insert(lowerCased);
                appendedCount++;
            }
        }

        size = keywordsToAdd.size();
        if (size > 0) {
            int rowsCount = m_KeywordsList.length();
            beginInsertRows(QModelIndex(), rowsCount, rowsCount + size - 1);

            for (int i = 0; i < size; ++i) {
                const QString &keywordToAdd = keywordsToAdd.at(i);
                m_KeywordsSet.insert(keywordToAdd.toLower());
                m_SpellCheckResults.append(true);
                m_KeywordsList.append(keywordToAdd);
            }

            endInsertRows();
        }

        return appendedCount;
    }

    bool BasicKeywordsModel::editKeyword(int index, const QString &replacement) {
        bool result = false;

        if (0 <= index && index < m_KeywordsList.length()) {
            QString sanitized = Helpers::doSanitizeKeyword(replacement);

            QString existing = m_KeywordsList.at(index);
            if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
                QString lowerCasedNew = sanitized.toLower();
                QString lowerCasedExisting = existing.toLower();

                if (!m_KeywordsSet.contains(lowerCasedNew)) {
                    m_KeywordsSet.insert(lowerCasedNew);
                    m_KeywordsList[index] = sanitized;
                    m_KeywordsSet.remove(lowerCasedExisting);

                    QModelIndex i = this->index(index);
                    emit dataChanged(i, i, QVector<int>() << KeywordRole);

                    result = true;
                } else if (lowerCasedNew == lowerCasedExisting) {
                    m_KeywordsList[index] = sanitized;
                    QModelIndex i = this->index(index);
                    emit dataChanged(i, i, QVector<int>() << KeywordRole);

                    result = true;
                }
                else {
                    qWarning() << "Attempt to rename keyword to existing one. Use remove instead";
                }
            }
        }

        return result;
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

    bool BasicKeywordsModel::isEmpty() const {
        return m_KeywordsList.isEmpty() || m_Description.simplified().isEmpty();
    }

    bool BasicKeywordsModel::isTitleEmpty() const {
        return m_Title.simplified().isEmpty();
    }

    bool BasicKeywordsModel::isDescriptionEmpty() const {
        return m_Description.simplified().isEmpty();
    }

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, bool exactMatch) {
        bool hasMatch = false;
        int length = m_KeywordsList.length();

        if (exactMatch) {
            for (int i = 0; i < length; ++i) {
                if (m_KeywordsList.at(i) == searchTerm) {
                    hasMatch = true;
                    break;
                }
            }
        } else {
            for (int i = 0; i < length; ++i) {
                if (m_KeywordsList.at(i).contains(searchTerm, Qt::CaseInsensitive)) {
                    hasMatch = true;
                    break;
                }
            }
        }

        return hasMatch;
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() const {
        bool anyError = false;
        int length = m_SpellCheckResults.length();

        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasDescriptionSpellError() const {
        bool hasError = m_SpellCheckInfo->anyDescriptionError();
        return hasError;
    }

    bool BasicKeywordsModel::hasTitleSpellError() const {
        bool hasError = m_SpellCheckInfo->anyTitleError();
        return hasError;
    }

    bool BasicKeywordsModel::hasSpellErrors() const {
        bool hasErrors = hasDescriptionSpellError() ||
                hasTitleSpellError() ||
                hasKeywordsSpellError();
        return hasErrors;
    }

    void BasicKeywordsModel::setSpellStatuses(const QVector<bool> &statuses) {
        Q_ASSERT(statuses.length() == m_SpellCheckResults.length());

        int size = statuses.length();
        for (int i = 0; i < size; ++i) {
            m_SpellCheckResults[i] = statuses[i];
        }
    }

    void BasicKeywordsModel::clearModel() {
        setDescription("");
        setTitle("");
        clearKeywords();
    }

    void BasicKeywordsModel::clearKeywords() {
        beginResetModel();
        m_KeywordsList.clear();
        endResetModel();

        m_SpellCheckResults.clear();
        m_KeywordsSet.clear();
    }

    void BasicKeywordsModel::resetKeywords(const QStringList &keywords) {
        clearKeywords();
        appendKeywords(keywords);
    }

    void BasicKeywordsModel::notifySpellCheckResults() {
        emit spellCheckResultsReady();
        emit spellCheckErrorsChanged();
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

    void BasicKeywordsModel::resetSpellCheckResults() {
        int size = m_SpellCheckResults.length();
        // TODO: use smth like memset
        for (int i = 0; i < size; ++i) {
            m_SpellCheckResults[i] = true;
        }
    }

    bool BasicKeywordsModel::canBeAdded(const QString &keyword) const {
        bool isValid = Helpers::isValidKeyword(keyword);
        bool result = isValid && !m_KeywordsSet.contains(keyword.toLower());
        return result;
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

    void BasicKeywordsModel::setSpellCheckResults(const QVector<SpellCheck::SpellCheckQueryItem *> &items, bool onlyOneKeyword) {
        Q_ASSERT(m_KeywordsList.length() == m_SpellCheckResults.length());

        if (!onlyOneKeyword) {
            resetSpellCheckResults();
        } else {
            Q_ASSERT(!items.isEmpty());
            int index = items.first()->m_Index;
            m_SpellCheckResults[index] = true;
        }

        int size = items.length();
        for (int i = 0; i < size; ++i) {
            SpellCheck::SpellCheckQueryItem *item = items.at(i);
            int index = item->m_Index;
            if (0 <= index && index < m_KeywordsList.length()) {
                if (m_KeywordsList[index].contains(item->m_Word)) {
                    // if keyword contains several words, there would be
                    // several queryitems and there's error if any has error
                    m_SpellCheckResults[index] = m_SpellCheckResults[index] && item->m_IsCorrect;
                }
            }
        }
    }

    void BasicKeywordsModel::setSpellCheckResults(const QHash<QString, bool> &results) {
        updateDescriptionSpellErrors(results);
        updateTitleSpellErrors(results);
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createKeywordsSuggestionsList() {
        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        int length = m_KeywordsList.length();
        spellCheckSuggestions.reserve(length/2);

        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                const QString &keyword = m_KeywordsList[i];

                if (!keyword.contains(QChar::Space)) {
                    SpellCheck::KeywordSpellSuggestions *suggestionsItem = new SpellCheck::KeywordSpellSuggestions(keyword, i);
                    spellCheckSuggestions.append(suggestionsItem);
                } else {
                    QStringList items = keyword.split(QChar::Space, QString::SkipEmptyParts);
                    foreach (const QString &item, items) {
                        SpellCheck::KeywordSpellSuggestions *suggestionsItem =
                                new SpellCheck::KeywordSpellSuggestions(item, i, keyword);
                        spellCheckSuggestions.append(suggestionsItem);
                    }
                }
            }
        }

        return spellCheckSuggestions;
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createDescriptionSuggestionsList() {
        QStringList descriptionErrors = m_SpellCheckInfo->retrieveDescriptionErrors();
        int length = descriptionErrors.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length);

        for (int i = 0; i < length; ++i) {
            const QString &word = descriptionErrors.at(i);
            SpellCheck::DescriptionSpellSuggestions *suggestionsItem = new SpellCheck::DescriptionSpellSuggestions(word);
            spellCheckSuggestions.append(suggestionsItem);
        }

        return spellCheckSuggestions;
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createTitleSuggestionsList() {
        QStringList titleErrors = m_SpellCheckInfo->retrieveTitleErrors();
        int length = titleErrors.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length);

        for (int i = 0; i < length; ++i) {
            const QString &word = titleErrors.at(i);
            SpellCheck::TitleSpellSuggestions *suggestionsItem = new SpellCheck::TitleSpellSuggestions(word);
            spellCheckSuggestions.append(suggestionsItem);
        }

        return spellCheckSuggestions;
    }

    void BasicKeywordsModel::replaceKeyword(int index, const QString &existing, const QString &replacement) {
        if (0 <= index && index < m_KeywordsList.length()) {
            const QString &internal = m_KeywordsList.at(index);
            if (internal == existing) {
                this->editKeyword(index, replacement);
                m_SpellCheckResults[index] = true;
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole);

            } else if (internal.contains(existing) && internal.contains(QChar::Space)) {
                QString existingFixed = internal;
                existingFixed.replace(existing, replacement);
                this->editKeyword(index, existingFixed);
                // TODO: reimplement this someday
                // no need to mark keyword as correct
                // if we replace only part of it
                m_SpellCheckResults[index] = true;
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole);
            }
        }
    }

    void BasicKeywordsModel::replaceWordInDescription(const QString &word, const QString &replacement) {
        QString description = m_Description;
        description.replace(word, replacement);
        setDescription(description);
    }

    void BasicKeywordsModel::replaceWordInTitle(const QString &word, const QString &replacement) {
        QString title = m_Title;
        title.replace(word, replacement);
        setTitle(title);
    }

    void BasicKeywordsModel::afterReplaceCallback() {
        emit spellCheckErrorsChanged();
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(int)), this, SLOT(spellCheckRequestReady(int)));
    }

    QStringList BasicKeywordsModel::getDescriptionWords() const {
        QStringList words;
        Helpers::splitText(m_Description, words);
        return words;
    }

    QStringList BasicKeywordsModel::getTitleWords() const {
        QStringList words;
        Helpers::splitText(m_Title, words);
        return words;
    }

    void BasicKeywordsModel::spellCheckRequestReady(int index) {
        emitSpellCheckChanged(index);
        notifySpellCheckResults();
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

    void BasicKeywordsModel::resetKeywords() {
        m_KeywordsList.clear();
        m_KeywordsSet.clear();
        m_SpellCheckResults.clear();
    }

    void BasicKeywordsModel::addKeywords(const QStringList &rawKeywords) {
        int size = rawKeywords.size();

        for (int i = 0; i < size; ++i) {
            const QString &keyword = rawKeywords.at(i).simplified();
            QString invariant = keyword.toLower();

            if (!m_KeywordsSet.contains(invariant)) {
                m_KeywordsList.append(keyword);
                m_SpellCheckResults.append(true);
                m_KeywordsSet.insert(invariant);
            } else {
                qWarning() << "Skipping duplicates in keywords...";
            }
        }
    }

    void BasicKeywordsModel::freeSpellCheckInfo() {
        if (m_SpellCheckInfo != NULL) {
            delete m_SpellCheckInfo;
        }
    }
}
