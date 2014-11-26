#include "artworkmetadata.h"

namespace Models {
    ArtworkMetadata::ArtworkMetadata(const QString &imageDescription, const QString &imageFileName,
                                 const QString &rawKeywords) {
        m_ImageFileName = imageFileName;
        m_ImageDescription = imageDescription;
        m_Keywords.parseKeywords(rawKeywords);
    }
}
