/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#include "combinededitcommand.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Models/artiteminfo.h"
#include "../Models/artworkmetadata.h"

Commands::CommandResult *Commands::CombinedEditCommand::execute(const Commands::CommandManager *commandManager) const
{
    QList<int> indicesToUpdate;
    QList<UndoRedo::ArtworkMetadataBackup*> artworksBackups;

    switch (m_EditType) {
    case AppendEditType:
        saveAppendKeywords(artworksBackups, indicesToUpdate);
        break;
    case SetEditType:
        saveSetKeywords(artworksBackups, indicesToUpdate);
        break;
    }

    UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
            new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate);
    commandManager->recordHistoryItem(modifyArtworksItem);

    CombinedEditCommandResult *result = new CombinedEditCommandResult(indicesToUpdate);
    return result;
}

void Commands::CombinedEditCommand::saveSetKeywords(QList<UndoRedo::ArtworkMetadataBackup *> &backups, QList<int> &indicesToUpdate) const
{
    foreach (Models::ArtItemInfo* info, m_ArtItemInfos) {
        Models::ArtworkMetadata *metadata = info->getOrigin();

        UndoRedo::ArtworkMetadataBackup *backup = new UndoRedo::ArtworkMetadataBackup(metadata);
        backups.append(backup);
        indicesToUpdate.append(info->getOriginalIndex());

        metadata->setKeywords(m_Keywords);
        metadata->setDescription(m_ArtworkDescription);
        metadata->setTitle(m_ArtworkTitle);
        metadata->setAuthor(m_ArtworkAuthor);
        metadata->saveBackup();
    }
}

void Commands::CombinedEditCommand::saveAppendKeywords(QList<UndoRedo::ArtworkMetadataBackup *> &backups, QList<int> &indicesToUpdate) const
{
    foreach (Models::ArtItemInfo* info, m_ArtItemInfos) {
        Models::ArtworkMetadata *metadata = info->getOrigin();

        UndoRedo::ArtworkMetadataBackup *backup = new UndoRedo::ArtworkMetadataBackup(metadata);
        backups.append(backup);
        indicesToUpdate.append(info->getOriginalIndex());

        metadata->appendKeywords(m_Keywords);

        if (!m_ArtworkDescription.isEmpty() && metadata->getDescription().isEmpty()) {
            metadata->setDescription(m_ArtworkDescription);
        }

        if (!m_ArtworkTitle.isEmpty() && metadata->getTitle().isEmpty()) {
            metadata->setTitle(m_ArtworkTitle);
        }

        if (!m_ArtworkAuthor.isEmpty() && metadata->getAuthor().isEmpty()) {
            metadata->setAuthor(m_ArtworkAuthor);
        }

        metadata->saveBackup();
    }
}
