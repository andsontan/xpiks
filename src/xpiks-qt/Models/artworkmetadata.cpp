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
#include "../SpellCheck/spellcheckiteminfo.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &filepath) :
        QAbstractListModel(),
        m_ArtworkFilepath(filepath),
        m_IsModified(false),
        m_IsSelected(false)
    {
        m_SpellCheckInfo = new SpellCheck::SpellCheckItemInfo();
    }

    ArtworkMetadata::~ArtworkMetadata() {
        this->disconnect();
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

    bool ArtworkMetadata::containsKeyword(const QString &searchTerm, bool exactMatch) {
        QReadLocker locker(&m_RWLock);

        bool hasMatch = false;

        if (exactMatch) {
            foreach (const QString &keyword, m_KeywordsList) {
                if (keyword == searchTerm) {
                    hasMatch = true;
                    break;
                }
            }
        } else {
            foreach (const QString &keyword, m_KeywordsList) {
                if (keyword.contains(searchTerm, Qt::CaseInsensitive)) {
                    hasMatch = true;
                    break;
                }
            }
        }

        return hasMatch;
    }

    bool ArtworkMetadata::hasKeywordsSpellError() {
        QReadLocker locker(&m_RWLock);

        bool anyError = false;

        foreach (bool isCorrect, m_SpellCheckResults) {
            if (!isCorrect) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool ArtworkMetadata::hasDescriptionSpellError() const {
        bool hasError = m_SpellCheckInfo->anyDescriptionError();
        return hasError;
    }

    bool ArtworkMetadata::hasTitleSpellError() const {
        bool hasError = m_SpellCheckInfo->anyTitleError();
        return hasError;
    }

    void ArtworkMetadata::setSpellCheckResults(const QList<SpellCheck::SpellCheckQueryItem *> &results) {
        QReadLocker locker(&m_RWLock);

        foreach (SpellCheck::SpellCheckQueryItem *item, results) {
            this->setSpellCheckResultUnsafe(item);
        }
    }

    void ArtworkMetadata::setSpellCheckResults(const QHash<QString, bool> &results) {
        updateDescriptionSpellErrors(results);
        updateTitleSpellErrors(results);

        emit spellCheckResultsReady();
    }

    void ArtworkMetadata::replaceKeyword(int index, const QString &existing, const QString &replacement) {
        QWriteLocker locker(&m_RWLock);

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

    QList<SpellCheck::KeywordSpellSuggestions *> ArtworkMetadata::createKeywordsSuggestionsList() {
        QReadLocker locker(&m_RWLock);

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

    void ArtworkMetadata::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(int)), this, SLOT(spellCheckRequestReady(int)));
    }

    QStringList ArtworkMetadata::getDescriptionWords() const {
        QStringList words = m_ArtworkDescription.split(" ", QString::SkipEmptyParts);
        return words;
    }

    QStringList ArtworkMetadata::getTitleWords() const {
        QStringList words = m_ArtworkTitle.split(" ", QString::SkipEmptyParts);
        return words;
    }

    void ArtworkMetadata::updateDescriptionSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> descriptionErrors;
        QStringList descriptionWords = getDescriptionWords();
        foreach (const QString &word, descriptionWords) {
            if (results.value(word, true) == false) {
                descriptionErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setDescriptionErrors(descriptionErrors);
    }

    void ArtworkMetadata::updateTitleSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> titleErrors;
        QStringList titleWords = getTitleWords();
        foreach (const QString &word, titleWords) {
            if (results.value(word, true) == false) {
                titleErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setTitleErrors(titleErrors);
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

            m_SpellCheckResults.removeAt(index);

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

            m_KeywordsSet.insert(sanitizedKeyword);
            m_SpellCheckResults.append(true);

            beginInsertRows(QModelIndex(), keywordsCount, keywordsCount);
            m_KeywordsList.append(sanitizedKeyword);
            endInsertRows();

            setModified();
            added = true;
        }

        return added;
    }

    void ArtworkMetadata::setSpellCheckResultUnsafe(SpellCheck::SpellCheckQueryItem *result) {
        int index = result->m_Index;

        if (0 <= index && index < m_SpellCheckResults.length()) {
            if (m_KeywordsList[index] == result->m_Word) {
                m_SpellCheckResults[index] = result->m_IsCorrect;
            }
        }
    }

    void ArtworkMetadata::emitSpellCheckChangedUnsafe(int index) {
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
        endResetModel();

        m_SpellCheckResults.clear();
        m_KeywordsSet.clear();
        setModified();
    }

    void ArtworkMetadata::addKeywords(const QString &rawKeywords) {
        QWriteLocker locker(&m_RWLock);
        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);

        for (int i = 0; i < keywordsList.size(); ++i) {
            const QString &keyword = keywordsList[i];
            m_KeywordsList.append(keyword);
            m_SpellCheckResults.append(true);
            m_KeywordsSet.insert(keyword.simplified().toLower());
        }
    }

    void ArtworkMetadata::saveBackup(SettingsModel *settings) {
        if (settings->getSaveBackups()) {
            QReadLocker locker(&m_RWLock);
            Helpers::TempMetadataDb(this).flush();
        }
    }

    void ArtworkMetadata::spellCheckRequestReady(int index) {
        qDebug() << "Spellcheck results ready. Index is" << index;
        emitSpellCheckChangedUnsafe(index);
    }

    int ArtworkMetadata::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_KeywordsList.count();
    }

    QVariant ArtworkMetadata::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_KeywordsList.count())
            return QVariant();

        int row = index.row();

        switch (role) {
        case KeywordRole:
            return m_KeywordsList.at(row);
        case IsCorrectRole:
            return m_SpellCheckResults.at(row);
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworkMetadata::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[IsCorrectRole] = "iscorrect";
        return roles;
    }
}
