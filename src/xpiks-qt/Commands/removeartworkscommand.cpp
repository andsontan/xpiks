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

#include <QVector>
#include <QPair>
#include <QString>
#include "removeartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/indiceshelper.h"
#include "../UndoRedo/removeartworksitem.h"
#include "../Common/defines.h"
#include "../Models/imageartwork.h"

namespace Commands {
    std::shared_ptr<ICommandResult> RemoveArtworksCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "removing" << m_RangesToRemove.length() << "ranges received";
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();

        int count = m_RangesToRemove.count();

        QVector<int> removedItemsIndices;
        removedItemsIndices.reserve(count);

        QStringList removedItemsFilepathes;
        QStringList removedAttachedVectors;
        removedItemsFilepathes.reserve(count);
        removedAttachedVectors.reserve(count);

        for (int k = 0; k < count; ++k) {
            const QPair<int, int> &item = m_RangesToRemove[k];
            int first = item.first;
            int last = item.second;

            for (int i = first; i <= last; ++i) {
                Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(i);
                if (metadata != NULL) {
                    removedItemsIndices.append(i);

                    if (!metadata->isUnavailable()) {
                        const QString &filepath = metadata->getFilepath();
                        removedItemsFilepathes.append(filepath);

                        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

                        if (image != NULL && image->hasVectorAttached()) {
                            removedAttachedVectors.append(image->getAttachedVectorPath());
                        } else {
                            removedAttachedVectors.append("");
                        }
                    }

                }
            }
        }

        int artworksToRemoveCount = removedItemsIndices.count();

        LOG_INFO << "removing" << artworksToRemoveCount << "real items found";

        if (artworksToRemoveCount > 0) {
            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(removedItemsIndices, rangesToRemove);
            artItemsModel->removeItemsAtIndices(rangesToRemove);

            commandManager->clearCurrentItem();

            Models::ArtworksRepository *artworkRepository = commandManager->getArtworksRepository();
            artworkRepository->cleanupEmptyDirectories();
            artworkRepository->updateCountsForExistingDirectories();
            artworkRepository->unwatchFilePaths(removedItemsFilepathes);

            QStringList notEmptyVectors = removedAttachedVectors;
            notEmptyVectors.removeAll("");
            artworkRepository->unwatchFilePaths(notEmptyVectors);

            artItemsModel->updateModifiedCount();

            if (!removedItemsFilepathes.empty()) {
                std::unique_ptr<UndoRedo::IHistoryItem> removeArtworksItem(
                        new UndoRedo::RemoveArtworksHistoryItem(getCommandID(),
                                                                removedItemsIndices,
                                                                removedItemsFilepathes,
                                                                removedAttachedVectors));
                commandManager->recordHistoryItem(removeArtworksItem);
            }

        } else {
            LOG_WARNING << "No items to remove found!";
        }

        // TODO: to be filled with useful return data in future
        std::shared_ptr<ICommandResult> result(new RemoveArtworksCommandResult(artworksToRemoveCount));
        return result;
    }
}
