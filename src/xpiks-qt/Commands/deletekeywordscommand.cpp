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

#include "deletekeywordscommand.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"

namespace Commands {
    DeleteKeywordsCommand::DeleteKeywordsCommand(std::vector<Models::MetadataElement> &infos,
                                                 const QSet<QString> &keywordsSet, bool caseSensitive):
        CommandBase(CommandType::DeleteKeywords),
        m_MetadataElements(std::move(infos)),
        m_KeywordsSet(keywordsSet),
        m_CaseSensitive(caseSensitive)
    {
    }

    std::shared_ptr<ICommandResult> DeleteKeywordsCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << m_KeywordsSet.size() << "keyword(s) to remove from" << m_MetadataElements.size() << "item(s)";
        LOG_INFO << "Case sensitive:" << m_CaseSensitive;
        QVector<int> indicesToUpdate;
        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        QVector<Models::ArtworkMetadata *> affectedItems;

        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        size_t size = m_MetadataElements.size();
        indicesToUpdate.reserve((int)size);
        artworksBackups.reserve(size);
        affectedItems.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            const Models::MetadataElement &info = m_MetadataElements.at(i);
            Models::ArtworkMetadata *metadata = info.getOrigin();

            artworksBackups.emplace_back(metadata);

            if (metadata->removeKeywords(m_KeywordsSet, m_CaseSensitive)) {
                indicesToUpdate.append(info.getOriginalIndex());
                affectedItems.append(metadata);
            } else {
                artworksBackups.pop_back();
            }
        }

        if (!artworksBackups.empty()) {
            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                                                                               UndoRedo::CombinedEditModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        }

        std::shared_ptr<ICommandResult> result(new DeleteKeywordsCommandResult(affectedItems, indicesToUpdate));
        return result;
    }

    void DeleteKeywordsCommandResult::afterExecCallback(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        if (!m_IndicesToUpdate.isEmpty()) {
            commandManager->updateArtworks(m_IndicesToUpdate);
        }

        if (!m_AffectedItems.isEmpty()) {
            commandManager->saveArtworksBackups(m_AffectedItems);
            commandManager->submitForSpellCheck(m_AffectedItems);
            commandManager->submitForWarningsCheck(m_AffectedItems);
        }
    }
}
