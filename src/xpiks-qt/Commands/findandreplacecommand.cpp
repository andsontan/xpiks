#include <QObject>
#include "findandreplacecommand.h"
#include "commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/artworkmetadata.h"
#include "../Models/artiteminfo.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"

namespace Commands {
    FindAndReplaceCommand::~FindAndReplaceCommand() {}

    QSharedPointer<Commands::ICommandResult> FindAndReplaceCommand::execute(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        QVector<UndoRedo::ArtworkMetadataBackup *> artworksBackups;
        QVector<int> indicesToUpdate;
        QVector<Models::ArtworkMetadata *> itemsToSave;

        int size = m_ArtItemInfos.size();
        itemsToSave.reserve(size);
        indicesToUpdate.reserve(size);

        for (int i = 0; i < size; i++) {
            Models::ArtworkMetadata *metadata = m_ArtItemInfos[i]->getOrigin();
            int index = m_ArtItemInfos[i]->getOriginalIndex();
            UndoRedo::ArtworkMetadataBackup *backup = new UndoRedo::ArtworkMetadataBackup(metadata);
            metadata->replace(m_ReplaceWhat, m_ReplaceTo, m_Flags);
            itemsToSave.append(metadata);
            artworksBackups.append(backup);
            indicesToUpdate.append(index);
        }

        if (indicesToUpdate.size() != 0) {
            UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
                new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                        UndoRedo::CombinedEditModificationType);
            commandManager->recordHistoryItem(modifyArtworksItem);
        }

        QSharedPointer<ICommandResult> result(new FindAndReplaceCommandResult(itemsToSave, indicesToUpdate));
        return result;
    }

    void FindAndReplaceCommandResult::afterExecCallback(const Commands::ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        if (!m_IndicesToUpdate.isEmpty()) {
            commandManager->updateArtworks(m_IndicesToUpdate);
        }

        if (!m_ItemsToSave.isEmpty()) {
            commandManager->saveArtworksBackups(m_ItemsToSave);
            commandManager->submitForSpellCheck(m_ItemsToSave);
            commandManager->submitForWarningsCheck(m_ItemsToSave);
        }
    }
}
