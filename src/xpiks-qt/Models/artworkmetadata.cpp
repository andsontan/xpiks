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

#include "artworkmetadata.h"
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include "../Helpers/tempmetadatadb.h"
#include "../Helpers/keywordvalidator.h"
#include "settingsmodel.h"
#include "../SpellCheck/keywordspellsuggestions.h"
#include "../SpellCheck/spellcheckitem.h"

namespace Models {
    ArtworkMetadata::~ArtworkMetadata() {
         this->disconnect();
        qDeleteAll(m_SpellCheckResults.values());
    }

    bool ArtworkMetadata::initialize(const QString &title,
                                     const QString &description, const QString &rawKeywords, bool overwrite) {
        bool anythingModified = false;

        if (overwrite || (m_ArtworkTitle.simplified().isEmpty() && !title.isEmpty())) {
            anythingModified = true;
            m_ArtworkTitle = title;
        }

        if (overwrite || (m_ArtworkDescription.simplified().isEmpty() && !description.isEmpty())) {
            anythingModified = true;
            m_ArtworkDescription = description;
        }

        if (overwrite && !rawKeywords.isEmpty()) {
            anythingModified = true;
            beginResetModel();

            m_RWLock.lockForWrite();
            {
                m_KeywordsList.clear();
                qDeleteAll(m_SpellCheckResults.values());
                m_SpellCheckResults.clear();
            }
            m_RWLock.unlock();

            addKeywords(rawKeywords);
            endResetModel();
        } else if (!rawKeywords.isEmpty()) {
            QStringList keywordsToAppend = rawKeywords.split(",", QString::SkipEmptyParts);
            int appendedCount = appendKeywords(keywordsToAppend);
            anythingModified = appendedCount > 0;
        }

        m_IsModified = false;

        return anythingModified;
    }

    int ArtworkMetadata::getKeywordsCount() {
        QReadLocker locker(&m_RWLock);
        int result = m_KeywordsSet.count();
        return result;
    }

    QStringList ArtworkMetadata::getKeywords() {
        QReadLocker locker(&m_RWLock);
        return QStringList(m_KeywordsList);
    }

    QString ArtworkMetadata::getKeywordsString() {
        QReadLocker locker(&m_RWLock);
        QString result =  m_KeywordsList.join(", ");
        return result;
    }

    bool ArtworkMetadata::isInDirectory(const QString &directory) const {
        bool startsWith = m_ArtworkFilepath.startsWith(directory);
        return startsWith;
    }

    bool ArtworkMetadata::isEmpty() const {
        return m_KeywordsList.isEmpty() || m_ArtworkDescription.simplified().isEmpty();
    }

    void ArtworkMetadata::clearMetadata() {
        setDescription("");
        setTitle("");

        QWriteLocker locker(&m_RWLock);
        resetKeywordsUnsafe();
    }

    QString ArtworkMetadata::retrieveKeyword(int index) {
        QString keyword = "";
        QReadLocker locker(&m_RWLock);

        if (0 <= index && index < m_KeywordsList.length()) {
            keyword = m_KeywordsList.at(index);
        }

        return keyword;
    }

    bool ArtworkMetadata::containsKeyword(const QString &searchTerm) {
        QReadLocker locker(&m_RWLock);

        bool hasMatch = false;

        foreach (const QString &keyword, m_KeywordsList) {
            if (keyword.contains(searchTerm, Qt::CaseInsensitive)) {
                hasMatch = true;
                break;
            }
        }

        return hasMatch;
    }

    void ArtworkMetadata::setSpellCheckResults(const QList<SpellCheck::SpellCheckQueryItem *> &results) {
        QReadLocker locker(&m_RWLock);

        foreach (SpellCheck::SpellCheckQueryItem *item, results) {
            this->setSpellCheckResultUnsafe(item);
        }

        int index = -1;

        if (results.length() == 1) {
            index = results.first()->m_Index;
        }

        emitSpellCheckChangedUnsafe(index);
    }

    bool ArtworkMetadata::hasAnySpellCheckError() {
        QReadLocker locker(&m_RWLock);

        bool anyError = false;

        QHashIterator<int, SpellCheck::SpellCheckQueryItem*> i(m_SpellCheckResults);

        while (i.hasNext()) {
            i.next();
            if (!i.value()->m_IsCorrect) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    void ArtworkMetadata::replaceKeyword(int index, const QString &existing, const QString &replacement) {
        QWriteLocker locker(&m_RWLock);

        if (0 <= index && index < m_KeywordsList.length()) {
            const QString &internal = m_KeywordsList.at(index);
            if (internal == existing) {
                m_KeywordsList[index] = replacement;
                m_SpellCheckResults[index]->m_IsCorrect = true;
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << SpellCheckOkRole);
            }
        }
    }

    QList<SpellCheck::KeywordSpellSuggestions *> ArtworkMetadata::createSuggestionsList() {
        QReadLocker locker(&m_RWLock);

        QList<SpellCheck::KeywordSpellSuggestions *> spellCheckSuggestions;

        int length = m_KeywordsList.length();
        for (int i = 0; i < length; ++i) {
            if (hasSpellCheckError(i)) {
                SpellCheck::SpellCheckQueryItem *item = m_SpellCheckResults.value(i, NULL);
                Q_ASSERT(item != NULL);
                const QStringList &suggestions = item->m_Suggestions;

                if (!suggestions.isEmpty()) {
                    const QString &keyword = m_KeywordsList[i];
                    SpellCheck::KeywordSpellSuggestions *suggestionsItem = new SpellCheck::KeywordSpellSuggestions(keyword, i);
                    suggestionsItem->setSuggestions(suggestions);
                    spellCheckSuggestions.append(suggestionsItem);
                }
            }
        }

        return spellCheckSuggestions;
    }

    bool ArtworkMetadata::removeKeywordAt(int index) {
        bool removed = false;
        QWriteLocker locker(&m_RWLock);

        if (index >= 0 && index < m_KeywordsList.length()) {
            const QString &keyword = m_KeywordsList.at(index);
            m_KeywordsSet.remove(keyword);

            beginRemoveRows(QModelIndex(), index, index);
            m_KeywordsList.removeAt(index);
            endRemoveRows();

            SpellCheck::SpellCheckQueryItem *item = m_SpellCheckResults.take(index);
            delete item;

            setModified();
            removed = true;
        }

        return removed;
    }

    bool ArtworkMetadata::appendKeyword(const QString &keyword) {
        QWriteLocker locker(&m_RWLock);
        bool result = appendKeywordUnsafe(keyword);
        return result;
    }

    bool ArtworkMetadata::appendKeywordUnsafe(const QString &keyword) {
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified().toLower();
        bool isValid = Helpers::isValidKeyword(sanitizedKeyword);
        if (isValid && !m_KeywordsSet.contains(sanitizedKeyword)) {
            int keywordsCount = m_KeywordsList.length();

            beginInsertRows(QModelIndex(), keywordsCount, keywordsCount);
            m_KeywordsList.append(sanitizedKeyword);
            endInsertRows();

            m_KeywordsSet.insert(sanitizedKeyword);
            setModified();
            added = true;
        }

        return added;
    }

    void ArtworkMetadata::setSpellCheckResultUnsafe(SpellCheck::SpellCheckQueryItem *result) {
        int index = result->m_Index;

        if (m_SpellCheckResults.contains(index)) {
            SpellCheck::SpellCheckQueryItem *prev = m_SpellCheckResults.value(index);
            qDebug() << "Replacing existing spellcheck item for word:" << prev->m_Word;
            delete prev;
        }

        m_SpellCheckResults.insert(index, new SpellCheck::SpellCheckQueryItem(*result));
    }

    void ArtworkMetadata::emitSpellCheckChangedUnsafe(int index) {
        int count = m_KeywordsList.length();

        if (index == -1) {
            if (count > 0) {
                QModelIndex start = this->index(0);
                QModelIndex end = this->index(count - 1);
                emit dataChanged(start, end, QVector<int>() << SpellCheckOkRole);
            }
        } else {
            if (0 <= index && index < count) {
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << SpellCheckOkRole);
            }
        }
    }

    void ArtworkMetadata::setKeywords(const QStringList &keywordsList) {
        QWriteLocker locker(&m_RWLock);
        resetKeywordsUnsafe();
        appendKeywordsUnsafe(keywordsList);
    }

    int ArtworkMetadata::appendKeywords(const QStringList &keywordsList) {
        QWriteLocker locker(&m_RWLock);
        int result = appendKeywordsUnsafe(keywordsList);
        return result;
    }

    int ArtworkMetadata::appendKeywordsUnsafe(const QStringList &keywordsList) {
        int appendedCount = 0;
        foreach (const QString &keyword, keywordsList) {
            if (appendKeywordUnsafe(keyword)) {
                appendedCount += 1;
            }
        }
        return appendedCount;
    }

    void ArtworkMetadata::resetKeywordsUnsafe() {
        beginResetModel();
        m_KeywordsList.clear();
        qDeleteAll(m_SpellCheckResults.values());
        m_SpellCheckResults.clear();
        endResetModel();

        m_KeywordsSet.clear();
        setModified();
    }

    bool ArtworkMetadata::hasSpellCheckError(int keywordIndex) const {
        bool hasError = false;
        SpellCheck::SpellCheckQueryItem *item = m_SpellCheckResults.value(keywordIndex, NULL);

        if (item != NULL) {
            hasError = (item->m_Word == m_KeywordsList[keywordIndex]) &&
                    (item->m_IsCorrect == false);
        }

        return hasError;
    }

    void ArtworkMetadata::addKeywords(const QString &rawKeywords) {
        QWriteLocker locker(&m_RWLock);
        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);

        for (int i = 0; i < keywordsList.size(); ++i) {
            const QString &keyword = keywordsList[i];
            m_KeywordsList.append(keyword);
            m_KeywordsSet.insert(keyword.simplified().toLower());
        }
    }

    void ArtworkMetadata::saveBackup(SettingsModel *settings) {
        if (settings->getSaveBackups()) {
            QReadLocker locker(&m_RWLock);
            Helpers::TempMetadataDb(this).flush();
        }
    }

    int ArtworkMetadata::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsList.count();
    }

    QVariant ArtworkMetadata::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_KeywordsList.count())
            return QVariant();

        switch (role) {
        case KeywordRole:
            return m_KeywordsList.at(index.row());
        case SpellCheckOkRole:
            return !hasSpellCheckError(index.row());
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworkMetadata::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[SpellCheckOkRole] = "spellcheckok";
        return roles;
    }
}
