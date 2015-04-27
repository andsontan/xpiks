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

#include "removeartworksitem.h"
#include "../Helpers/indiceshelper.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"

void UndoRedo::RemoveArtworksHistoryItem::undo(const Commands::CommandManager *commandManager) const {
    qDebug() << "Undo: remove artworks command";

    QList<QPair<int, int> > ranges;
    Helpers::indicesToRanges(m_RemovedArtworksIndices, ranges);

    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();

    QList<Models::ArtworkMetadata*> artworksToImport;

    bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_RemovedArtworksPathes);

    int usedCount = 0;
    int rangesCount = ranges.count();
    for (int i = 0; i < rangesCount; ++i) {
        int startRow = ranges[i].first;
        int endRow = ranges[i].second;

        artItemsModel->beginAccountingFiles(startRow, endRow);

        int count = endRow - startRow + 1;
        for (int j = 0; j < count; ++j) {
            const QString &filepath = m_RemovedArtworksPathes[j + usedCount];

            if (artworksRepository->accountFile(filepath)) {
                Models::ArtworkMetadata *metadata = new Models::ArtworkMetadata(filepath);
                commandManager->connectArtworkSignals(metadata);

                artItemsModel->insertArtwork(j + startRow, metadata);
                artworksToImport.append(metadata);
            }
        }

        artItemsModel->endAccountingFiles();

        usedCount += (endRow - startRow + 1);
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);

    commandManager->setArtworksForIPTCProcessing(artworksToImport);
    artItemsModel->raiseArtworksAdded(usedCount);
}
