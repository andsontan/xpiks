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

#include "basickeywordsmodel.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordvalidator.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(Hold &hold, QObject *parent):
        AbstractListModel(parent),
        m_Hold(hold),
        m_SpellCheckInfo(NULL)
    {
    }

    void BasicKeywordsModel::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        AbstractListModel::doRemoveItemsAtIndices(ranges);
    }

    void BasicKeywordsModel::removeInnerItem(int row) {
        QString removedKeyword;
        bool wasCorrect = false;
        this->takeKeywordAtUnsafe(row, removedKeyword, wasCorrect);
#ifdef INTEGRATION_TESTS
        LOG_DEBUG << "keyword:" << removedKeyword << "was correct:" << wasCorrect;
#endif
    }

    int BasicKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);

        //QReadLocker readLocker(&m_KeywordsLock);
        //Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there

        return m_KeywordsList.length();
    }

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        //QReadLocker readLocker(&m_KeywordsLock);
        //Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there

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

    QString BasicKeywordsModel::getDescription() {
        QReadLocker readLocker(&m_DescriptionLock);
        Q_UNUSED(readLocker);

        return m_Description;
    }

    QString BasicKeywordsModel::getTitle() {
        QReadLocker readLocker(&m_TitleLock);
        Q_UNUSED(readLocker);

        return m_Title;
    }

    int BasicKeywordsModel::getKeywordsCount() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsSet.count();
    }

    QSet<QString> BasicKeywordsModel::getKeywordsSet() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsSet;
    }

    QString BasicKeywordsModel::getKeywordsString() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsList.join(", ");
    }

    bool BasicKeywordsModel::appendKeyword(const QString &keyword) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        return appendKeywordUnsafe(keyword);
    }

    bool BasicKeywordsModel::takeKeywordAt(int index, QString &removedKeyword) {
        bool wasCorrect = false, result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                takeKeywordAtUnsafe(index, removedKeyword, wasCorrect);
                result = true;
            }
        }
        m_KeywordsLock.unlock();

        if (!wasCorrect) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::takeLastKeyword(QString &removedKeyword) {
        bool wasCorrect = false, result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (m_KeywordsList.length() > 0) {
                takeKeywordAtUnsafe(m_KeywordsList.length() - 1, removedKeyword, wasCorrect);
                result = true;
            }
        }
        m_KeywordsLock.unlock();

        if (!wasCorrect) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    void BasicKeywordsModel::setKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        setKeywordsUnsafe(keywordsList);
    }

    int BasicKeywordsModel::appendKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        return appendKeywordsUnsafe(keywordsList);
    }

    bool BasicKeywordsModel::editKeyword(int index, const QString &replacement) {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                result = editKeywordUnsafe(index, replacement);
            } else {
                LOG_WARNING << "Failed to edit keyword with index" << index;
            }
        }
        m_KeywordsLock.unlock();

        if (result) {
            QModelIndex i = this->index(index);
            emit dataChanged(i, i, QVector<int>() << KeywordRole);
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywords() {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            result = clearKeywordsUnsafe();
        }
        m_KeywordsLock.unlock();

        if (result) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::areKeywordsEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsList.isEmpty();
    }

    bool BasicKeywordsModel::appendKeywordUnsafe(const QString &keyword) {
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified();

        if (canBeAddedUnsafe(sanitizedKeyword)) {
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

    void BasicKeywordsModel::takeKeywordAtUnsafe(int index, QString &removedKeyword, bool &wasCorrect) {
        const QString &keyword = m_KeywordsList.at(index);
        QString invariant = keyword.toLower();
        m_KeywordsSet.remove(invariant);

        beginRemoveRows(QModelIndex(), index, index);
        removedKeyword = m_KeywordsList.takeAt(index);
        wasCorrect = m_SpellCheckResults.takeAt(index);
        endRemoveRows();
    }

    void BasicKeywordsModel::setKeywordsUnsafe(const QStringList &keywordsList) {
        clearKeywordsUnsafe();
        appendKeywordsUnsafe(keywordsList);
    }

    int BasicKeywordsModel::appendKeywordsUnsafe(const QStringList &keywordsList) {
        QStringList keywordsToAdd;
        int appendedCount = 0, size = keywordsList.length();
        keywordsToAdd.reserve(size);
        QSet<QString> accountedKeywords;

        for (int i = 0; i < size; ++i) {
            const QString &keyword = keywordsList.at(i);
            const QString &sanitizedKeyword = keyword.simplified();
            const QString &lowerCased = sanitizedKeyword.toLower();

            if (canBeAddedUnsafe(sanitizedKeyword) && !accountedKeywords.contains(lowerCased)) {
                keywordsToAdd.append(sanitizedKeyword);
                accountedKeywords.insert(lowerCased);
                appendedCount++;
            }
        }

        size = keywordsToAdd.size();
        Q_ASSERT(size == appendedCount);

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

    bool BasicKeywordsModel::editKeywordUnsafe(int index, const QString &replacement) {
        bool result = false;
        QString sanitized = Helpers::doSanitizeKeyword(replacement);

        QString existing = m_KeywordsList.at(index);
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            QString lowerCasedNew = sanitized.toLower();
            QString lowerCasedExisting = existing.toLower();

            if (!m_KeywordsSet.contains(lowerCasedNew)) {
                m_KeywordsSet.insert(lowerCasedNew);
                m_KeywordsList[index] = sanitized;
                m_KeywordsSet.remove(lowerCasedExisting);
                LOG_DEBUG << "common case edit";

                result = true;
            } else if (lowerCasedNew == lowerCasedExisting) {
                LOG_DEBUG << "changing case in same keyword";
                m_KeywordsList[index] = sanitized;

                result = true;
            }
            else {
                LOG_WARNING << "Attempt to rename keyword to existing one. Use remove instead!";
            }
        }

        return result;
    }

    bool BasicKeywordsModel::replaceKeywordUnsafe(int index, const QString &existing, const QString &replacement) {
        bool result = false;

        const QString &internal = m_KeywordsList.at(index);
        if (internal == existing) {
            if (this->editKeywordUnsafe(index, replacement)) {
                m_SpellCheckResults[index] = true;
                result = true;
            }
        } else if (internal.contains(existing) && internal.contains(QChar::Space)) {
            LOG_DEBUG << "Replacing composite keyword";
            QString existingFixed = internal;
            existingFixed.replace(existing, replacement);
            if (this->editKeywordUnsafe(index, existingFixed)) {
                // TODO: reimplement this someday
                // no need to mark keyword as correct
                // if we replace only part of it
                m_SpellCheckResults[index] = true;
                result = true;
            }
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywordsUnsafe() {
        bool anyKeywords = !m_KeywordsList.isEmpty();

        if (anyKeywords) {
            beginResetModel();
            m_KeywordsList.clear();
            endResetModel();

            m_SpellCheckResults.clear();
            m_KeywordsSet.clear();
        } else {
            Q_ASSERT(m_KeywordsSet.isEmpty());
            Q_ASSERT(m_SpellCheckResults.isEmpty());
        }

        return anyKeywords;
    }

    bool BasicKeywordsModel::containsKeywordUnsafe(const QString &searchTerm, bool exactMatch) {
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

    bool BasicKeywordsModel::hasKeywordsSpellErrorUnsafe() const {
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

    bool BasicKeywordsModel::setDescription(const QString &value) {
        QWriteLocker writeLocker(&m_DescriptionLock);
        Q_UNUSED(writeLocker);

        bool result = value != m_Description;
        if (result) { m_Description = value; }
        return result;
    }

    bool BasicKeywordsModel::setTitle(const QString &value) {
        QWriteLocker writeLocker(&m_TitleLock);
        Q_UNUSED(writeLocker);

        bool result = value != m_Title;
        if (result) { m_Title = value; }
        return result;
    }

    bool BasicKeywordsModel::isEmpty() {
        QReadLocker readKeywordsLock(&m_KeywordsLock);
        Q_UNUSED(readKeywordsLock);

        QReadLocker readDescriptionLock(&m_DescriptionLock);
        Q_UNUSED(readDescriptionLock);

        return m_KeywordsList.isEmpty() || m_Description.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::isTitleEmpty() {
        QReadLocker readLocker(&m_TitleLock);
        Q_UNUSED(readLocker);

        return m_Title.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::isDescriptionEmpty() {
        QReadLocker readLocker(&m_DescriptionLock);
        Q_UNUSED(readLocker);

        return m_Description.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, bool exactMatch) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return containsKeywordUnsafe(searchTerm, exactMatch);
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return hasKeywordsSpellErrorUnsafe();
    }

    bool BasicKeywordsModel::hasDescriptionSpellError() {
        bool anyError = false;

        const QStringList &descriptionWords = getDescriptionWords();
        foreach (const QString &word, descriptionWords) {
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasTitleSpellError() {
        bool anyError = false;

        const QStringList &titleWords = getTitleWords();
        foreach (const QString &word, titleWords) {
            if (m_SpellCheckInfo->hasTitleError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasSpellErrors() {
        bool hasErrors = hasDescriptionSpellError() ||
                hasTitleSpellError() ||
                hasKeywordsSpellError();
        return hasErrors;
    }

    void BasicKeywordsModel::setSpellStatuses(BasicKeywordsModel *keywordsModel) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        keywordsModel->lockKeywordsRead();
        {
            const QVector<bool> &spellStatuses = keywordsModel->getSpellStatusesUnsafe();

#ifdef QT_DEBUG
            // sync issue between adding/removing/undo/spellcheck
            Q_ASSERT(spellStatuses.length() == m_SpellCheckResults.length());
#endif

            int size = qMin(spellStatuses.length(), m_SpellCheckResults.length());

            for (int i = 0; i < size; ++i) {
                m_SpellCheckResults[i] = spellStatuses[i];
            }
        }
        keywordsModel->unlockKeywords();
    }

    void BasicKeywordsModel::clearModel() {
        setDescription("");
        setTitle("");
        clearKeywords();
    }

    void BasicKeywordsModel::notifySpellCheckResults(int flags) {
        if (Common::HasFlag(flags, Common::SpellCheckDescription) ||
                Common::HasFlag(flags, Common::SpellCheckTitle)) {
            emit spellCheckResultsReady();
        }
        
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

    void BasicKeywordsModel::resetSpellCheckResultsUnsafe() {
        int size = m_SpellCheckResults.length();
        // TODO: use smth like memset
        for (int i = 0; i < size; ++i) {
            m_SpellCheckResults[i] = true;
        }
    }

    bool BasicKeywordsModel::canBeAddedUnsafe(const QString &keyword) const {
        bool isValid = Helpers::isValidKeyword(keyword);
        bool result = isValid && !m_KeywordsSet.contains(keyword.toLower());
        return result;
    }

    bool BasicKeywordsModel::hasKeyword(const QString &keyword) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return canBeAddedUnsafe(keyword.simplified());
    }

    QString BasicKeywordsModel::retrieveKeyword(int wordIndex) {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        QString keyword;
        if (0 <= wordIndex && wordIndex < m_KeywordsList.length()) {
            keyword = m_KeywordsList.at(wordIndex);
        }

        return keyword;
    }

    QStringList BasicKeywordsModel::getKeywords() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

        return m_KeywordsList;
    }

    void BasicKeywordsModel::setSpellCheckResults(const QVector<SpellCheck::SpellCheckQueryItem *> &items, bool onlyOneKeyword) {
        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

#ifdef INTEGRATION_TESTS
        if (m_KeywordsList.length() != m_SpellCheckResults.length()) {
            LOG_DEBUG << "Current keywords list length:" << m_KeywordsList.length();
            LOG_DEBUG << "SpellCheck list length:" << m_SpellCheckResults.length();
        }
#endif
        // sync issue between adding/removing/undo/spellcheck
        Q_ASSERT(m_KeywordsList.length() == m_SpellCheckResults.length());

        if (!onlyOneKeyword) {
            resetSpellCheckResultsUnsafe();
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

    void BasicKeywordsModel::setSpellCheckResults(const QHash<QString, bool> &results, int flags) {
        if (Common::HasFlag(flags, Common::SpellCheckDescription)) {
            updateDescriptionSpellErrors(results);
        }

        if (Common::HasFlag(flags, Common::SpellCheckTitle)) {
            updateTitleSpellErrors(results);
        }
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createKeywordsSuggestionsList() {
        QReadLocker readLocker(&m_KeywordsLock);
        Q_UNUSED(readLocker);

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
        QStringList descriptionWords = getDescriptionWords();
        int length = descriptionWords.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = descriptionWords.at(i);
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                SpellCheck::DescriptionSpellSuggestions *suggestionsItem = new SpellCheck::DescriptionSpellSuggestions(word);
                spellCheckSuggestions.append(suggestionsItem);
            }
        }

        return spellCheckSuggestions;
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createTitleSuggestionsList() {
        QStringList titleWords = getTitleWords();
        int length = titleWords.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = titleWords.at(i);
            if (m_SpellCheckInfo->hasTitleError(word)) {
                SpellCheck::TitleSpellSuggestions *suggestionsItem = new SpellCheck::TitleSpellSuggestions(word);
                spellCheckSuggestions.append(suggestionsItem);
            }
        }

        return spellCheckSuggestions;
    }

    bool BasicKeywordsModel::replaceKeyword(int index, const QString &existing, const QString &replacement) {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            LOG_DEBUG << "Replacing" << existing << "to" << replacement << "with index" << index;
            if (0 <= index && index < m_KeywordsList.length()) {
                result = replaceKeywordUnsafe(index, existing, replacement);
            } else {
                LOG_DEBUG << "Failure. Index is negative or exceeds count" << m_KeywordsList.length();
            }
        }
        m_KeywordsLock.unlock();

        if (result) {
            QModelIndex i = this->index(index);
            // combined roles from legacy editKeyword() and replace()
            emit dataChanged(i, i, QVector<int>() << KeywordRole << IsCorrectRole);
        }

        return result;
    }

    void BasicKeywordsModel::replaceWordInDescription(const QString &word, const QString &replacement) {
        QString description = getDescription();
        description.replace(word, replacement);
        setDescription(description);
    }

    void BasicKeywordsModel::replaceWordInTitle(const QString &word, const QString &replacement) {
        QString title = getTitle();
        title.replace(word, replacement);
        setTitle(title);
    }

    void BasicKeywordsModel::afterReplaceCallback() {
        emit spellCheckErrorsChanged();
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(int, int)), this, SLOT(spellCheckRequestReady(int, int)));
    }

    QStringList BasicKeywordsModel::getDescriptionWords() {
        QReadLocker readLocker(&m_DescriptionLock);
        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Description, words);
        return words;
    }

    QStringList BasicKeywordsModel::getTitleWords() {
        QReadLocker readLocker(&m_TitleLock);
        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Title, words);
        return words;
    }

    void BasicKeywordsModel::spellCheckRequestReady(int flags, int index) {
        if (Common::HasFlag(flags, Common::SpellCheckKeywords)) {
            emitSpellCheckChanged(index);
        }

        notifySpellCheckResults(flags);
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
