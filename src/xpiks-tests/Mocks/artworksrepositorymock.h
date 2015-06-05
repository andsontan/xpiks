#ifndef ARTWORKSREPOSITORYMOCK_H
#define ARTWORKSREPOSITORYMOCK_H

#include "../xpiks-qt/Models/artworksrepository.h"

namespace Mocks {
    class ArtworksRepositoryMock : public Models::ArtworksRepository
    {
    public:
        ArtworksRepositoryMock() {}

    public:
        virtual int getNewDirectoriesCount(const QStringList &items) const { return items.count(); }

    protected:
        virtual bool checkFileExists(const QString &filename, QString &directory) const {
            Q_UNUSED(filename);
            directory = "somedirectory";
            return true;
        }
    };
}

#endif // ARTWORKSREPOSITORYMOCK_H
