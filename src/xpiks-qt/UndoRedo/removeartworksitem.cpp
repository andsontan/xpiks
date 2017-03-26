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

#include <QFileInfo>
#include "removeartworksitem.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "addartworksitem.h"

void UndoRedo::RemoveArtworksHistoryItem::undo(const Commands::ICommandManager *commandManagerInterface) const {
    LOG_INFO << "#";

    Commands::CommandManager *commandManager = (Commands::CommandManager*)commandManagerInterface;

    QVector<QPair<int, int> > ranges;
    Helpers::indicesToRanges(m_RemovedArtworksIndices, ranges);

    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();

    QVector<Models::ArtworkMetadata*> artworksToImport;
    artworksToImport.reserve(m_RemovedArtworksIndices.length());
    QStringList watchList;
    watchList.reserve(m_RemovedArtworksIndices.length());

    bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_RemovedArtworksPathes);
    bool willResetModel = m_RemovedArtworksPathes.length() > 50;

    int usedCount = 0, attachedVectors = 0;
    int rangesCount = ranges.count();

    if (willResetModel) {
        artItemsModel->beginAccountingManyFiles();
    }

    for (int i = 0; i < rangesCount; ++i) {
        int startRow = ranges[i].first;
        int endRow = ranges[i].second;

        if (!willResetModel) {
            artItemsModel->beginAccountingFiles(startRow, endRow);
        }

        int count = endRow - startRow + 1;
        for (int j = 0; j < count; ++j) {
            const QString &filepath = m_RemovedArtworksPathes[j + usedCount];
            qint64 directoryID = 0;
            if (artworksRepository->accountFile(filepath, directoryID)) {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filepath, directoryID);
                commandManager->connectArtworkSignals(metadata);

                artItemsModel->insertArtwork(j + startRow, metadata);
                artworksToImport.append(metadata);
                watchList.append(filepath);

                const QString &vectorPath = m_RemovedAttachedVectors.at(j);
                if (!vectorPath.isEmpty()) {
                    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);
                    if (image != NULL) {
                        image->attachVector(vectorPath);
                        attachedVectors++;
                    }
                    else {
                        LOG_WARNING << "Vector was attached not to an image!";
                    }
                }
            }
        }

        if (!willResetModel) {
            artItemsModel->endAccountingFiles();
        }

        usedCount += (endRow - startRow + 1);
    }

    if (willResetModel) {
        artItemsModel->endAccountingManyFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);
    artworksRepository->watchFilePaths(watchList);
    artworksRepository->updateFilesCounts();

    std::unique_ptr<IHistoryItem> addArtworksItem(new AddArtworksHistoryItem(getCommandID(), ranges));
    commandManager->recordHistoryItem(addArtworksItem);

    commandManager->readMetadata(artworksToImport, ranges);
    artItemsModel->raiseArtworksAdded(usedCount, attachedVectors);
}
