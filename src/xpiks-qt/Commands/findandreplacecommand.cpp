#include <QObject>
#include "findandreplacecommand.h"
#include "commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/artworkmetadata.h"
#include "../Models/metadataelement.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"

namespace Commands {
    FindAndReplaceCommand::~FindAndReplaceCommand() { LOG_DEBUG << "#"; }

    std::shared_ptr<Commands::ICommandResult> FindAndReplaceCommand::execute(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        QVector<int> indicesToUpdate;
        QVector<Models::ArtworkMetadata *> itemsToSave;

        size_t size = m_MetadataElements.size();
        itemsToSave.reserve((int)size);
        indicesToUpdate.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            const Models::MetadataElement &element = m_MetadataElements.at(i);
            Models::ArtworkMetadata *metadata = element.getOrigin();
            int index = element.getOriginalIndex();

            artworksBackups.emplace_back(metadata);

            metadata->replace(m_ReplaceWhat, m_ReplaceTo, m_Flags);

            itemsToSave.append(metadata);
            indicesToUpdate.append(index);
        }

        if (indicesToUpdate.size() != 0) {
            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                                                                               UndoRedo::CombinedEditModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        }

        std::shared_ptr<ICommandResult> result(new FindAndReplaceCommandResult(itemsToSave, indicesToUpdate));
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
