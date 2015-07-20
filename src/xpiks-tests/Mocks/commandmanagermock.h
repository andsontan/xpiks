#ifndef COMMANDMANAGERMOCK_H
#define COMMANDMANAGERMOCK_H

#include "../xpiks-qt/Commands/commandmanager.h"
#include "../xpiks-qt/Models/artitemsmodel.h"
#include "../xpiks-qt/Models/artworksrepository.h"
#include "artworksrepositorymock.h"

namespace Mocks {
    class CommandManagerMock : public Commands::CommandManager
    {
    public:
        CommandManagerMock() {}

    public:
        virtual void connectArtworkSignals(Models::ArtworkMetadata *metadata) const { Q_UNUSED(metadata); /*DO NOTHING*/ }
        void generateAndAddArtworks(int count) {
            Q_ASSERT(count >= 0);
            int i = 0;
            Models::ArtworksRepository *artworksRepository = getArtworksRepository();
            Models::ArtItemsModel *artItemsModel = getArtItemsModel();

            while (i < count) {
                QString filename = QString("somedirectory/artwork%1.jpg").arg(i);

                if (artworksRepository->accountFile(filename))
                {
                    Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename);
                    artItemsModel->appendMetadata(metadata);
                }

                i++;
            }
        }
    };
}

#endif // COMMANDMANAGERMOCK_H
