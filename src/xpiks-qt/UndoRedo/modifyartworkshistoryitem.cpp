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

#include "modifyartworkshistoryitem.h"
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Commands/commandmanager.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

void UndoRedo::ModifyArtworksHistoryItem::undo(const Commands::ICommandManager *commandManagerInterface) const {
    LOG_INFO << m_Indices.count() << "item(s) affected";

    Commands::CommandManager *commandManager = (Commands::CommandManager*)commandManagerInterface;

    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    int count = m_Indices.count();

    QVector<Models::ArtworkMetadata*> itemsToSave;
    itemsToSave.reserve(count);

    for (int i = 0; i < count; ++i) {
        int index = m_Indices[i];
        Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(index);
        if (metadata != NULL) {
            const ArtworkMetadataBackup &backup = m_ArtworksBackups.at(i);
            backup.restore(metadata);
            itemsToSave.append(metadata);
        }
    }

    commandManager->submitForSpellCheck(itemsToSave);
    commandManager->saveArtworksBackups(itemsToSave);
    artItemsModel->updateItemsAtIndices(m_Indices);
    artItemsModel->updateModifiedCount();
}

QString UndoRedo::getModificationTypeDescription(UndoRedo::ModificationType type) {
    switch (type) {
    case PasteModificationType:
        return QObject::tr("Paste");
    case CombinedEditModificationType:
        return QObject::tr("Multiple edit");
    default:
        return QLatin1String("");
    }
}
