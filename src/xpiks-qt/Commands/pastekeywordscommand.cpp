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

#include <QList>
#include "pastekeywordscommand.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../Models/artiteminfo.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "../Models/artitemsmodel.h"

Commands::PasteKeywordsCommand::~PasteKeywordsCommand() {
    LOG_DEBUG << "#";
    qDeleteAll(m_ArtItemInfos);
}

QSharedPointer<Commands::ICommandResult> Commands::PasteKeywordsCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << "Pasting" << m_KeywordsList.length() << "keywords to" << m_ArtItemInfos.length() << "item(s)";

    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    QVector<int> indicesToUpdate;
    QVector<UndoRedo::ArtworkMetadataBackup*> artworksBackups;
    QVector<Models::ArtworkMetadata*> itemsToSave;
    int size = m_ArtItemInfos.length();
    indicesToUpdate.reserve(size);
    artworksBackups.reserve(size);
    itemsToSave.reserve(size);

    for (int i = 0; i < size; ++i) {
        Models::ArtItemInfo *itemInfo = m_ArtItemInfos.at(i);
        Models::ArtworkMetadata *metadata = itemInfo->getOrigin();

        indicesToUpdate.append(itemInfo->getOriginalIndex());
        artworksBackups.append(new UndoRedo::ArtworkMetadataBackup(metadata));

        metadata->appendKeywords(m_KeywordsList);
        itemsToSave.append(metadata);
    }

    if (size > 0) {
        commandManager->submitForSpellCheck(itemsToSave);
        commandManager->submitForWarningsCheck(itemsToSave);
        commandManager->saveArtworksBackups(itemsToSave);

        UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
                new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                        UndoRedo::PasteModificationType);
        commandManager->recordHistoryItem(modifyArtworksItem);
    } else {
        LOG_WARNING << "Pasted zero real words!";
    }

    QSharedPointer<PasteKeywordsCommandResult> result(new PasteKeywordsCommandResult(indicesToUpdate));
    return result;
}

void Commands::PasteKeywordsCommandResult::afterExecCallback(const Commands::ICommandManager *commandManagerInterface) const {
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    artItemsModel->updateItems(m_IndicesToUpdate,
                               QVector<int>() <<
                               Models::ArtItemsModel::IsModifiedRole <<
                               Models::ArtItemsModel::KeywordsCountRole);
}
