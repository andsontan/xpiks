#ifndef ARTWORKSREPOSITORYMOCK_H
#define ARTWORKSREPOSITORYMOCK_H

#include <QVector>
#include <QPair>
#include "../xpiks-qt/Models/artworksrepository.h"

namespace Mocks {
    class ArtWorksRepositoryMock : public Models::ArtworksRepository
    {
    public:
        ArtWorksRepositoryMock() {}


    public:
        void RemoveFilesAndEmitSignal(int num){

             Models::ArtworksRepository::removeFilesAndEmitSignal(num);
        }

};

}


#endif // ARTWORKSREPOSITORYMOCK_H

