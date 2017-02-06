/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Models/metadataelement.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "../Models/artitemsmodel.h"

Commands::PasteKeywordsCommand::~PasteKeywordsCommand() {
    LOG_DEBUG << "#";
}

std::shared_ptr<Commands::ICommandResult> Commands::PasteKeywordsCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << "Pasting" << m_KeywordsList.length() << "keywords to" << m_MetadataElements.size() << "item(s)";

    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    QVector<int> indicesToUpdate;
    std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
    QVector<Models::ArtworkMetadata*> affectedArtworks;
    size_t size = m_MetadataElements.size();
    indicesToUpdate.reserve((int)size);
    artworksBackups.reserve(size);
    affectedArtworks.reserve((int)size);

    for (size_t i = 0; i < size; ++i) {
        const Models::MetadataElement &element = m_MetadataElements.at(i);
        Models::ArtworkMetadata *metadata = element.getOrigin();

        indicesToUpdate.append(element.getOriginalIndex());
        artworksBackups.emplace_back(metadata);

        metadata->appendKeywords(m_KeywordsList);
        affectedArtworks.append(metadata);
    }

    if (size > 0) {
        commandManager->submitForSpellCheck(affectedArtworks);
        commandManager->submitForWarningsCheck(affectedArtworks);
        commandManager->saveArtworksBackups(affectedArtworks);

        std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                                                                           UndoRedo::PasteModificationType));
        commandManager->recordHistoryItem(modifyArtworksItem);
    } else {
        LOG_WARNING << "Pasted zero real words!";
    }

    std::shared_ptr<PasteKeywordsCommandResult> result(new PasteKeywordsCommandResult(indicesToUpdate));
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
