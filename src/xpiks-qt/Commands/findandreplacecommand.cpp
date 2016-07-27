/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "findandreplacecommand.h"
#include <QObject>
#include "commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/artworkmetadata.h"
#include "../Models/previewmetadataelement.h"
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
            const Models::PreviewMetadataElement &element = m_MetadataElements.at(i);
            if (element.isReplacable() == false) {
                continue;
            }

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
