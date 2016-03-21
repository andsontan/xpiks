#ifndef COMMANDMANAGERMOCK_H
#define COMMANDMANAGERMOCK_H

#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"

namespace Mocks {
    class CommandManagerMock : public Commands::CommandManager
    {
    public:
        CommandManagerMock():
            m_AnyCommandProcessed(false),
            m_CanExecuteCommands(true)
        {}

    public:
        bool anyCommandProcessed() const { return m_AnyCommandProcessed; }
        void resetAnyCommandProcessed() { m_AnyCommandProcessed = false; }
        void disableCommands() { m_CanExecuteCommands = false; }
        void mockAcceptDeletion() {Commands::CommandManager::removeUnavailableFiles();}

    public:
        virtual void connectArtworkSignals(Models::ArtworkMetadata *metadata) const { Q_UNUSED(metadata); /*DO NOTHING*/ }

        void generateAndAddArtworks(int count, bool withVector=true) {
            Q_ASSERT(count >= 0);
            int i = 0;
            Models::ArtworksRepository *artworksRepository = getArtworksRepository();
            Models::ArtItemsModel *artItemsModel = getArtItemsModel();

            while (i < count) {
                QString filename = QString("/path/to/somedirectory/artwork%1.jpg").arg(i);
                QString vectorname = QString("/path/to/somedirectory/artwork%1.eps").arg(i);

                if (artworksRepository->accountFile(filename)) {
                    Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename);

                    if (withVector) {
                        metadata->attachVector(vectorname);
                    }

                    artItemsModel->appendMetadata(metadata);
                }

                i++;
            }
        }

        virtual Commands::ICommandResult *processCommand(Commands::ICommandBase *command) {
            m_AnyCommandProcessed = true;
            if (m_CanExecuteCommands) {
                return Commands::CommandManager::processCommand(command);
            } else {
                return NULL;
            }
        }

     void mockDeletion(int count) {
       for (int i =0; i < count; ++i) {
            CommandManager::getArtItemsModel()->getArtwork(i)->setUnavailable();
       }
     }

    private:
        bool m_AnyCommandProcessed;
        volatile bool m_CanExecuteCommands;
    };
}

#endif // COMMANDMANAGERMOCK_H
