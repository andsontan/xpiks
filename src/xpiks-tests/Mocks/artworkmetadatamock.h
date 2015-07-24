#ifndef ARTWORKMETADATAMOCK
#define ARTWORKMETADATAMOCK

#include <QString>
#include "../xpiks-qt/Models/artworkmetadata.h"

namespace Mocks {
    class ArtworkMetadataMock : public Models::ArtworkMetadata {
    public:
        ArtworkMetadataMock(const QString &filepath):
            Models::ArtworkMetadata(filepath)
        {
        }

        virtual QString getDirectory() const { return "somedirectory"; }
    };
}

#endif // ARTWORKMETADATAMOCK

