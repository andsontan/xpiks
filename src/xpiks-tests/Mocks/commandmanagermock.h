#ifndef COMMANDMANAGERMOCK_H
#define COMMANDMANAGERMOCK_H

#include "../xpiks-qt/Commands/commandmanager.h"
#include "artworksrepositorymock.h"

namespace Mocks {
    class CommandManagerMock : public Commands::CommandManager
    {
    public:
        CommandManagerMock() {}

    public:
        virtual void connectArtworkSignals(Models::ArtworkMetadata *metadata) const { Q_UNUSED(metadata); /*DO NOTHING*/ }
    };
}

#endif // COMMANDMANAGERMOCK_H
