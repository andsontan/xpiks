#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

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
        //const KeywordsModel &GetKeywords() const { return m_Keywords; }
        bool isInDirectory(const QString &directory) const { return m_ImageFileName.startsWith(directory); }

    private:
        KeywordsModel m_Keywords;
        QString m_ImageFileName;
        QString m_ImageDescription;
    };
}

#endif // IMAGEMETADATA_H
