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
#include <QString>
#include <QStringList>
#include "../Commands/commandmanager.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Models/metadataelement.h"
#include "../Models/artworkmetadata.h"
#include "../Common/flags.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

QString combinedFlagsToString(int flags) {
    if (flags == Common::EditEverything) {
        return QLatin1String("EditEverything");
    }

    QStringList flagsStr;
    if (Common::HasFlag(flags, Common::EditDesctiption)) {
        flagsStr.append("EditDescription");
    }

    if (Common::HasFlag(flags, Common::EditTitle)) {
        flagsStr.append("EditTitle");
    }

    if (Common::HasFlag(flags, Common::EditKeywords)) {
        flagsStr.append("EditKeywords");
    }

    if (Common::HasFlag(flags, Common::Clear)) {
        flagsStr.append("Clear");
    }

    return flagsStr.join('|');
}

Commands::CombinedEditCommand::~CombinedEditCommand() {
    LOG_DEBUG << "#";
}

QSharedPointer<Commands::ICommandResult> Commands::CombinedEditCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << "flags =" << combinedFlagsToString(m_EditFlags) << ", artworks count =" << m_MetadataElements.length();
    QVector<int> indicesToUpdate;
    QVector<UndoRedo::ArtworkMetadataBackup*> artworksBackups;
    QVector<Models::ArtworkMetadata *> itemsToSave, affectedItems;

    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    int size = m_MetadataElements.length();
    indicesToUpdate.reserve(size);
    artworksBackups.reserve(size);
    itemsToSave.reserve(size);
    affectedItems.reserve(size);

    bool needToClear = Common::HasFlag(m_EditFlags, Common::Clear);

    for (int i = 0; i < size; ++i) {
        Models::MetadataElement* info = m_MetadataElements[i];
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
            itemsToSave.append(metadata);
        }

        affectedItems.append(metadata);
    }

    UndoRedo::ModifyArtworksHistoryItem *modifyArtworksItem =
            new UndoRedo::ModifyArtworksHistoryItem(artworksBackups, indicesToUpdate,
                                                    UndoRedo::CombinedEditModificationType);
    commandManager->recordHistoryItem(modifyArtworksItem);

    QSharedPointer<ICommandResult> result(new CombinedEditCommandResult(affectedItems, itemsToSave, indicesToUpdate));
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

void Commands::CombinedEditCommandResult::afterExecCallback(const Commands::ICommandManager *commandManagerInterface) const {
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    if (!m_IndicesToUpdate.isEmpty()) {
        commandManager->updateArtworks(m_IndicesToUpdate);
    }

    if (!m_ItemsToSave.isEmpty()) {
        commandManager->saveArtworksBackups(m_ItemsToSave);
    }

    if (!m_AffectedItems.isEmpty()) {
        commandManager->submitForSpellCheck(m_AffectedItems);
        commandManager->submitForWarningsCheck(m_AffectedItems);
    }
}
