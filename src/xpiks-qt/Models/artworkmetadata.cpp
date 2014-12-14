#include "artworkmetadata.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &imageDescription, const QString &imageFileName,
                                 const QString &rawKeywords) {
        m_ImageFileName = imageFileName;
        m_ImageDescription = imageDescription;
        m_IsModified = false;
        parseKeywords(rawKeywords);
    }

    void ArtworkMetadata::parseKeywords(const QString &rawKeywords) {
        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);

        for (int i = 0; i < keywordsList.size(); ++i) {
            const QString &keyword = keywordsList[i].trimmed().toLower();
            m_KeywordsList.append(keyword);
            m_KeywordsSet.insert(keyword.trimmed().toLower());
        }
    }
}
