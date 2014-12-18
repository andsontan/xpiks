#include "artworkmetadata.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &imageDescription, const QString &imageFileName,
                                 const QString &rawKeywords) {
        m_ImageFileName = imageFileName;
        m_ImageDescription = imageDescription;
        m_IsModified = false;
        m_IsSelected = false;
        parseKeywords(rawKeywords);
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
        const QString &sanitizedKeyword = keyword.trimmed().toLower();
        if (!m_KeywordsSet.contains(sanitizedKeyword)) {
            m_KeywordsList.append(keyword);
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

    void ArtworkMetadata::parseKeywords(const QString &rawKeywords) {
        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);
        for (int i = 0; i < keywordsList.size(); ++i) {
            const QString &keyword = keywordsList[i];
            m_KeywordsList.append(keyword);
            m_KeywordsSet.insert(keyword.trimmed().toLower());
        }
    }
}
