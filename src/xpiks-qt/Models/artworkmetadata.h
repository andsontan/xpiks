#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QSet>
#include <QString>
#include "keywordsmodel.h"

namespace Models {
    class ArtworkMetadata {
    public:
        ArtworkMetadata(const QString &imageDescription, const QString &imageFileName,
                      const QString &rawKeywords);

    public:
        const QString &GetImageDescription() const { return m_ImageDescription; }
        const QString &GetImageFileName() const { return m_ImageFileName; }
        const QStringList &GetKeywords() const { return m_KeywordsList; }
        bool isInDirectory(const QString &directory) const { return m_ImageFileName.startsWith(directory); }

    private:
        void parseKeywords(const QString& rawKeywords);

    private:
        QStringList m_KeywordsList;
        QSet<QString> m_KeywordsSet;
        QString m_ImageFileName;
        QString m_ImageDescription;
    };
}

#endif // IMAGEMETADATA_H
