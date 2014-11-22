#include <QStringList>
#include <QJsonObject>
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
                      const QString &rawCategories, const QString &shutterstockID);

    private:
        QSet<QString> m_KeywordsSet;
        QStringList m_Categories;
        qulonglong m_ShutterstockID; // optional
        QString m_ImageFileName;
        QString m_ImageDescription;
    };
}

#endif // IMAGEMETADATA_H
