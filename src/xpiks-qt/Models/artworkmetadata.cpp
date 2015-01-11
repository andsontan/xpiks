/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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
    void ArtworkMetadata::initialize(const QString &author, const QString &title,
                                     const QString &description, const QString &rawKeywords)
    {
        m_ArtworkAuthor = author;
        m_ArtworkTitle = title;
        m_ArtworkDescription = description;
        addKeywords(rawKeywords);
    }

    bool ArtworkMetadata::removeKeywordAt(int index)
    {
        bool removed = false;
        if (index >= 0 && index < m_KeywordsList.length()) {
            const QString &keyword = m_KeywordsList.at(index);
            m_KeywordsSet.remove(keyword);
            m_KeywordsList.removeAt(index);
            setModified();
            removed = true;
        }

        return removed;
    }

    bool ArtworkMetadata::appendKeyword(const QString &keyword)
    {
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified().toLower();
        if (!m_KeywordsSet.contains(sanitizedKeyword)) {
            m_KeywordsList.append(sanitizedKeyword);
            m_KeywordsSet.insert(sanitizedKeyword);
            setModified();
            added = true;
        }

        return added;
    }

    void ArtworkMetadata::appendKeywords(const QStringList &keywordsList)
    {
        foreach (const QString &keyword, keywordsList) {
            appendKeyword(keyword);
        }
    }

    void ArtworkMetadata::resetKeywords()
    {
        m_KeywordsList.clear();
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

    void ArtworkMetadata::saveBackup()
    {
        qDebug() << "Backing up item... " << this->getFilepath();
        Helpers::TempMetadataDb(this).flush();
    }
}
