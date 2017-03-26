#ifndef COMMANDMANAGERMOCK_H
#define COMMANDMANAGERMOCK_H

#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/imageartwork.h"

#ifdef Q_OS_WIN
#define ARTWORK_PATH "C:/path/to/somedirectory_%1/artwork%2.jpg"
#define VECTOR_PATH "C:/path/to/somedirectory_%1/artwork%2.eps"
#else
#define ARTWORK_PATH "/path/to/somedirectory_%1/artwork%2.jpg"
#define VECTOR_PATH "/path/to/somedirectory_%1/artwork%2.eps"
#endif

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
        void enableCommands() { m_CanExecuteCommands = true; }
        void mockAcceptDeletion() {Commands::CommandManager::removeUnavailableFiles();}

    public:
        void generateAndAddArtworks(size_t count, bool withVector=true) {
            Q_ASSERT(count >= 0);
            size_t i = 0;
            Models::ArtworksRepository *artworksRepository = getArtworksRepository();
            Models::ArtItemsModel *artItemsModel = getArtItemsModel();

            while (i < count) {
                QString filename = QString(ARTWORK_PATH).arg(i%2).arg(i);
                QString vectorname = QString(VECTOR_PATH).arg(i%2).arg(i);
                qint64 directoryID;

                if (artworksRepository->accountFile(filename, directoryID)) {
                    Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename, directoryID);
                    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

                    this->connectArtworkSignals(metadata);

                    if (withVector) {
                        image->attachVector(vectorname);
                    }

                    artItemsModel->appendMetadata(metadata);
                }

                i++;
            }
        }

        virtual std::shared_ptr<Commands::ICommandResult> processCommand(const std::shared_ptr<Commands::ICommandBase> &command) {
            m_AnyCommandProcessed = true;
            if (m_CanExecuteCommands) {
                return Commands::CommandManager::processCommand(command);
            } else {
                return std::shared_ptr<Commands::ICommandResult>();
            }
        }

        void mockDeletion(int count) {
            for (int i = 0; i < count; ++i) {
                CommandManager::getArtItemsModel()->getArtwork(i)->setUnavailable();
            }
        }

    private:
        bool m_AnyCommandProcessed;
        volatile bool m_CanExecuteCommands;
    };
}

#endif // COMMANDMANAGERMOCK_H
