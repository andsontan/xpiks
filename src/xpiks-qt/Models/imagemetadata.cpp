#include "imagemetadata.h"

namespace Models
{
    ImageMetadata::ImageMetadata(const QString &imageDescription, const QString &imageFileName,
                                 const QString &rawKeywords, const QString &rawCategories)
    {
        m_ImageFileName = imageFileName;
        m_ImageDescription = imageDescription;

        m_Categories = rawCategories.split(",", QString::SkipEmptyParts);

        QStringList keywordsList = rawKeywords.split(",", QString::SkipEmptyParts);
        for (int i = 0; i < keywordsList.size(); ++i)
        {
            const QString &keyword = keywordsList[i];
            m_KeywordsSet.insert(keyword.trimmed().toLower());
        }
    }
}
