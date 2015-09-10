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

#include <QDebug>
#include "artworkmetadata.h"
#include "../Helpers/tempmetadatadb.h"

namespace Models {
    bool ArtworkMetadata::initialize(const QString &author, const QString &title,
                                     const QString &description, const QString &rawKeywords, bool overwrite) {
        bool anythingModified = false;

        if (overwrite || (m_ArtworkAuthor.trimmed().isEmpty() && !author.isEmpty())) {
            anythingModified = true;
            m_ArtworkAuthor = author;
        }

        if (overwrite || (m_ArtworkTitle.trimmed().isEmpty() && !title.isEmpty())) {
            anythingModified = true;
            m_ArtworkTitle = title;
        }

        if (overwrite || (m_ArtworkDescription.trimmed().isEmpty() && !description.isEmpty())) {
            anythingModified = true;
            m_ArtworkDescription = description;
        }

        if (overwrite || (m_KeywordsList.empty() && !rawKeywords.isEmpty())) {
            anythingModified = true;
            beginResetModel();
            m_KeywordsList.clear();
            addKeywords(rawKeywords);
            endResetModel();
        }

        return anythingModified;
    }

    bool ArtworkMetadata::isEmpty() const {
        return m_KeywordsList.isEmpty() || m_ArtworkDescription.isEmpty();
    }

    bool ArtworkMetadata::removeKeywordAt(int index) {
        bool removed = false;
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
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified().toLower();
        if (!sanitizedKeyword.isEmpty() && !m_KeywordsSet.contains(sanitizedKeyword)) {
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

    void ArtworkMetadata::appendKeywords(const QStringList &keywordsList) {
        foreach (const QString &keyword, keywordsList) {
            appendKeyword(keyword);
        }
    }

    void ArtworkMetadata::resetKeywords() {
        beginResetModel();
        m_KeywordsList.clear();
        endResetModel();

        m_KeywordsSet.clear();
        setModified();
    }

    void ArtworkMetadata::addKeywords(const QString &rawKeywords) {
        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);

        for (int i = 0; i < keywordsList.size(); ++i) {
            const QString &keyword = keywordsList[i];
            m_KeywordsList.append(keyword);
            m_KeywordsSet.insert(keyword.trimmed().toLower());
        }
    }

    void ArtworkMetadata::saveBackup() {
        Helpers::TempMetadataDb(this).flush();
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
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> ArtworkMetadata::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        return roles;
    }
}
