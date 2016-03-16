#ifndef ARTWORKSREPOSITORYMOCK_H
#define ARTWORKSREPOSITORYMOCK_H

#include <QVector>
#include <QPair>
#include "../xpiks-qt/Models/artworksrepository.h"

namespace Mocks {
    class ArtWorksRepositoryMock : public Models::ArtworksRepository{
    public:
        ArtWorksRepositoryMock() {}

        void publicRemoveFileAndEmitSignal(){
             Models::ArtworksRepository::removeFileAndEmitSignal();
        }
    };
}
#endif // ARTWORKSREPOSITORYMOCK_H

