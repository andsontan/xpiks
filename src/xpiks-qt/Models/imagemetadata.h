#include <QStringList>
#include <QString>
#include <QSet>

#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

namespace Models
{
    class ImageMetadata
    {
    public:
        ImageMetadata(const QString &imageDescription, const QString &imageFileName, const QString &rawKeywords,
                      const QString &rawCategories);

    private:
        QSet<QString> m_KeywordsSet;
        QStringList m_Categories;
        QString m_ImageFileName;
        QString m_ImageDescription;
    };
}

#endif // IMAGEMETADATA_H
