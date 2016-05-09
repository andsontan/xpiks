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

        QString retrieveKeyword(int index) { return getKeywordsModel()->retrieveKeyword(index); }
        int rowCount() const { return getKeywordsModel()->rowCount(); }

        bool isTitleEmpty() { return getKeywordsModel()->isTitleEmpty(); }
        bool isDescriptionEmpty() { return getKeywordsModel()->isDescriptionEmpty(); }

        bool areKeywordsEmpty() { return getKeywordsModel()->areKeywordsEmpty(); }

        //virtual QString getDirectory() const { return "somedirectory"; }
    };
}

#endif // ARTWORKMETADATAMOCK

