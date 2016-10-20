#ifndef ARTWORKMETADATAMOCK
#define ARTWORKMETADATAMOCK

#include <QString>
#include "../../xpiks-qt/Models/imageartwork.h"

namespace Mocks {
    class ArtworkMetadataMock : public Models::ImageArtwork {
    public:
        ArtworkMetadataMock(const QString &filepath):
            Models::ImageArtwork(filepath, 0)
        {
        }

        QString retrieveKeyword(int index) { return getBasicModel()->retrieveKeyword(index); }
        int rowCount() const { return getBasicModel()->rowCount(); }

        bool isTitleEmpty() { return getBasicModel()->isTitleEmpty(); }
        bool isDescriptionEmpty() { return getBasicModel()->isDescriptionEmpty(); }

        bool areKeywordsEmpty() { return getBasicModel()->areKeywordsEmpty(); }

        //virtual QString getDirectory() const { return "somedirectory"; }
    };
}

#endif // ARTWORKMETADATAMOCK

