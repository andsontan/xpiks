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

#ifndef WARNINGSINFO_H
#define WARNINGSINFO_H

#include <QStringList>
#include <QString>
#include <QImageReader>
#include "artiteminfo.h"

namespace Models {
    class WarningsInfo
    {
    public:
        WarningsInfo(ArtItemInfo *itemInfo):
            m_ArtworkInfo(itemInfo)
        {
            updateData();
        }

        ~WarningsInfo() { delete m_ArtworkInfo; }

    public:
        void addWarning(const QString &warning) { m_WarningsList.append(warning); }
        //ArtworkMetadata *getArtworkMetadata() const { return m_ArtworkInfo->getOrigin(); }
        void clearWarnings() { m_WarningsList.clear(); }
        bool hasWarnings() const { return m_WarningsList.length() > 0; }

        void updateData() {
            ArtworkMetadata *metadata = m_ArtworkInfo->getOrigin();
            m_Filepath = metadata->getFilepath();
            if (metadata->isInitialized()) {
                m_Dimensions = metadata->getImageSize();
            } else {
                QImageReader reader(m_Filepath);
                m_Dimensions = reader.size();
            }
            m_ArtworkIndex = m_ArtworkInfo->getOriginalIndex();
            m_KeywordsCount = metadata->getKeywordsCount();
            m_DescriptionLength = metadata->getDescription().length();
            m_TitleLength = metadata->getTitle().length();
            m_TitleSpellErrors = metadata->hasTitleSpellError();
            m_DescriptionSpellErrors = metadata->hasDescriptionSpellError();
            m_KeywordsSpellErrors = metadata->hasKeywordsSpellError();
            m_DescriptionWordsCount = calculateWordsLength(metadata->getDescriptionWords());
            m_TitleWordsCount = calculateWordsLength(metadata->getTitleWords());
        }

    public:
        const QString &getFilePath() const { return m_Filepath; }
        const QStringList &getWarnings() const { return m_WarningsList; }
        int getIndex() const { return m_ArtworkIndex; }
        QSize getSize() const { return m_Dimensions; }
        int getDescriptionLength() const { return m_DescriptionLength; }
        int getTitleLength() const { return m_TitleLength; }
        int getKeywordsCount() const { return m_KeywordsCount; }
        bool hasTitleSpellErrors() const { return m_TitleSpellErrors; }
        bool hasDescriptionSpellErrors() const { return m_DescriptionSpellErrors; }
        bool hasKeywordsSpellErrors() const { return m_KeywordsSpellErrors; }
        int getDescriptionWordsCount() const { return m_DescriptionWordsCount; }
        int getTitleWordsCount() const { return m_TitleWordsCount; }

    private:
        int calculateWordsLength(const QStringList &stringList) const {
            int wordsLength = 0;

            foreach (const QString &part, stringList) {
                if (part.length() > 2) {
                    wordsLength++;
                }
            }

            return wordsLength;
        }

    private:
        ArtItemInfo *m_ArtworkInfo;
        QStringList m_WarningsList;
        QString m_Filepath;
        QSize m_Dimensions;
        int m_ArtworkIndex;
        int m_KeywordsCount;
        int m_DescriptionLength;
        int m_DescriptionWordsCount;
        int m_TitleLength;
        int m_TitleWordsCount;
        bool m_TitleSpellErrors;
        bool m_DescriptionSpellErrors;
        bool m_KeywordsSpellErrors;
    };
}

#endif // WARNINGSINFO_H
