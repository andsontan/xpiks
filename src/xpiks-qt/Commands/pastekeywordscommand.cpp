/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QDebug>
#include <QList>
#include "pastekeywordscommand.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../Models/artiteminfo.h"
#include "../Commands/commandmanager.h"

Commands::PasteKeywordsCommand::~PasteKeywordsCommand() {
    qDeleteAll(m_ArtItemInfos);
}

Commands::CommandResult *Commands::PasteKeywordsCommand::execute(const Commands::CommandManager *commandManager) const {
    qInfo() << "Paste command: pasting to" << m_ArtItemInfos.length() << "item(s)";

    QVector<int> indicesToUpdate;
    QVector<UndoRedo::ArtworkMetadataBackup*> artworksBackups;
    QVector<SpellCheck::ISpellCheckable*> itemsToCheck;
    indicesToUpdate.reserve(m_ArtItemInfos.length());
    artworksBackups.reserve(m_ArtItemInfos.length());
    itemsToCheck.reserve(m_ArtItemInfos.length());

    foreach (Models::ArtItemInfo *itemInfo, m_ArtItemInfos) {
        Models::ArtworkMetadata *metadata = itemInfo->getOrigin();

        indicesToUpdate.append(itemInfo->getOriginalIndex());
        artworksBackups.append(new UndoRedo::ArtworkMetadataBackup(metadata));

        metadata->appendKeywords(m_KeywordsList);
        commandManager->saveMetadata(metadata);
    }

    commandManager->submitForSpellCheck(itemsToCheck);

    UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
            new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                    UndoRedo::PasteModificationType);
    commandManager->recordHistoryItem(modifyArtworksItem);

    PasteKeywordsCommandResult *result = new PasteKeywordsCommandResult();
    return result;
}
