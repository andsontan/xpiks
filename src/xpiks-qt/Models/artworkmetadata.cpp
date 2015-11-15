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

namespace Models {
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

        QWriteLocker locker(&m_RWLock);

        if (overwrite && !rawKeywords.isEmpty()) {
            anythingModified = true;
            beginResetModel();
            m_KeywordsList.clear();
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

    bool ArtworkMetadata::removeKeywordAt(int index) {
        bool removed = false;
        QWriteLocker locker(&m_RWLock);

        if (index >= 0 && index < m_KeywordsList.length()) {
            const QString &keyword = m_KeywordsList.at(index);
            m_KeywordsSet.remove(keyword);

            beginRemoveRows(QModelIndex(), index, index);
            m_KeywordsList.removeAt(index);
            endRemoveRows();

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

        m_KeywordsSet.clear();
        setModified();
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
            return true;
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
