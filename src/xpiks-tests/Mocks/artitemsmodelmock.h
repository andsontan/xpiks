#ifndef ARTITEMSMODELMOCK_H
#define ARTITEMSMODELMOCK_H

#include "../xpiks-qt/Models/artitemsmodel.h"
#include "../xpiks-qt/Models/artworkmetadata.h"
#include "artworkmetadatamock.h"

namespace Mocks {
    class ArtItemsModelMock : public Models::ArtItemsModel
    {
    public:
        ArtItemsModelMock() {}

    public:
        virtual Models::ArtworkMetadata *createMetadata(const QString &filepath) const {
            ArtworkMetadataMock *metadata = new ArtworkMetadataMock(filepath);
            return metadata;
        }

    };
}

#endif // ARTITEMSMODELMOCK_H
