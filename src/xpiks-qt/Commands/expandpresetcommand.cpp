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

#include "expandpresetcommand.h"
#include "commandmanager.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Models/artitemsmodel.h"
#include "../Helpers/stringhelper.h"

namespace Commands {
    ExpandPresetCommand::~ExpandPresetCommand() {
    }

    std::shared_ptr<ICommandResult> ExpandPresetCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "Expand preset" << m_PresetIndex;

        CommandManager *commandManager = (CommandManager*)commandManagerInterface;
        auto *presetsModel = commandManager->getPresetsModel();
        QStringList keywords;

        int indexToUpdate = -1;
        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        QVector<Models::ArtworkMetadata*> affectedArtworks;

        if (presetsModel->tryGetPreset(m_PresetIndex, keywords)) {
            Models::ArtworkMetadata *metadata = m_MetadataElement.getOrigin();

            indexToUpdate = m_MetadataElement.getOriginalIndex();
            artworksBackups.emplace_back(metadata);

            bool useMerging = false;
            QStringList artworkKeywords;

#ifdef KEYWORDS_TAGS
            artworkKeywords = metadata->getKeywords();
            useMerging = Helpers::hasTaggedKeywords(keywords) || Helpers::hasTaggedKeywords(artworkKeywords);
#endif

            if (!useMerging) {
                if (m_KeywordIndex != -1) {
                    if (metadata->expandPreset(m_KeywordIndex, keywords)) {
                        affectedArtworks.append(metadata);
                    }
                } else {
                    if (metadata->appendKeywords(keywords)) {
                        affectedArtworks.append(metadata);
                    }
                }
            } else {
                QStringList merged = Helpers::mergeTaggedLists(artworkKeywords, keywords);
                metadata->setKeywords(merged);
            }
        }

        if (affectedArtworks.size() > 0) {
            commandManager->submitForSpellCheck(affectedArtworks);
            commandManager->submitForWarningsCheck(affectedArtworks);
            commandManager->saveArtworksBackups(affectedArtworks);

            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(new UndoRedo::ModifyArtworksHistoryItem(artworksBackups,
                                                                                                               QVector<int>() << indexToUpdate,
                                                                                                               UndoRedo::PasteModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        } else {
            LOG_WARNING << "Failed to expand preset";
        }

        std::shared_ptr<ExpandPresetCommandResult> result(new ExpandPresetCommandResult(indexToUpdate));
        return result;
    }

    void ExpandPresetCommandResult::afterExecCallback(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;
        Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
        artItemsModel->updateItems(QVector<int>() << m_IndexToUpdate,
                                   QVector<int>() <<
                                   Models::ArtItemsModel::IsModifiedRole <<
                                   Models::ArtItemsModel::KeywordsCountRole);
    }
}
