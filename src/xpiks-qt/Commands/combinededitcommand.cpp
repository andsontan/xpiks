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

#include "combinededitcommand.h"
#include <QVector>
#include <QDebug>
#include "../Commands/commandmanager.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Models/artiteminfo.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

Commands::CommandResult *Commands::CombinedEditCommand::execute(const Commands::CommandManager *commandManager) const {
    qInfo() << "Combined edit command: flags=" << m_EditFlags << "artworks count =" << m_ArtItemInfos.length();
    QVector<int> indicesToUpdate;
    QVector<UndoRedo::ArtworkMetadataBackup*> artworksBackups;
    QVector<SpellCheck::ISpellCheckable*> itemsToCheck;

    int size = m_ArtItemInfos.length();
    indicesToUpdate.reserve(size);
    artworksBackups.reserve(size);
    itemsToCheck.reserve(size);

    bool needToClear = Common::HasFlag(m_EditFlags, Common::Clear);

    for (int i = 0; i < size; ++i) {
        Models::ArtItemInfo* info = m_ArtItemInfos[i];
        Models::ArtworkMetadata *metadata = info->getOrigin();

        UndoRedo::ArtworkMetadataBackup *backup = new UndoRedo::ArtworkMetadataBackup(metadata);
        artworksBackups.append(backup);
        indicesToUpdate.append(info->getOriginalIndex());

        setKeywords(metadata);
        setDescription(metadata);
        setTitle(metadata);

        // do not save if Сlear flag present
        // to be able to restore from .xpks
        if (!needToClear) {
            commandManager->saveMetadata(metadata);
        }

        itemsToCheck.append(metadata);
    }

    commandManager->submitForSpellCheck(itemsToCheck);

    UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
            new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                    UndoRedo::CombinedEditModificationType);
    commandManager->recordHistoryItem(modifyArtworksItem);

    CombinedEditCommandResult *result = new CombinedEditCommandResult(indicesToUpdate);
    return result;
}

void Commands::CombinedEditCommand::setKeywords(Models::ArtworkMetadata *metadata) const {
    if (Common::HasFlag(m_EditFlags, Common::EditKeywords)) {
        if (Common::HasFlag(m_EditFlags, Common::AppendKeywords)) {
            metadata->appendKeywords(m_Keywords);
        }
        else {
            if (Common::HasFlag(m_EditFlags, Common::Clear)) {
                metadata->clearKeywords();
                metadata->markModified();
            } else {
                metadata->setKeywords(m_Keywords);
            }
        }
    }
}

void Commands::CombinedEditCommand::setDescription(Models::ArtworkMetadata *metadata) const {
    if (Common::HasFlag(m_EditFlags, Common::EditDesctiption)) {
        if (Common::HasFlag(m_EditFlags, Common::Clear)) {
            metadata->setDescription("");
        } else {
            metadata->setDescription(m_ArtworkDescription);
        }
    }
}

void Commands::CombinedEditCommand::setTitle(Models::ArtworkMetadata *metadata) const {
    if (Common::HasFlag(m_EditFlags, Common::EditTitle)) {
        if (Common::HasFlag(m_EditFlags, Common::Clear)) {
            metadata->setTitle("");
        } else {
            metadata->setTitle(m_ArtworkTitle);
        }
    }
}
