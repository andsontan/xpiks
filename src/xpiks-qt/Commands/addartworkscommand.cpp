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

#include <QFileInfo>
#include <QVector>
#include <QHash>
#include <memory>
#include "addartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artworkmetadata.h"
#include "../Models/artitemsmodel.h"
#include "../UndoRedo/addartworksitem.h"
#include "../Common/defines.h"
#include "../Helpers/filenameshelpers.h"
#include "../Models/imageartwork.h"

int findAndAttachVectors(const QVector<Models::ArtworkMetadata*> &artworksList, QVector<int> &modifiedIndices) {
    LOG_DEBUG << "#";
    int attachedCount = 0;
    int size = artworksList.length();
    modifiedIndices.reserve(size);

    for (int i = 0; i < size; ++i) {
        Models::ArtworkMetadata *metadata = artworksList.at(i);
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);

        if (image == NULL) { continue; }

        if (image->hasVectorAttached()) {
            attachedCount++;
            modifiedIndices.append(i);
            continue;
        }

        const QString &filepath = image->getFilepath();
        QStringList vectors = Helpers::convertToVectorFilenames(QStringList() << filepath);

        foreach (const QString &item, vectors) {
            if (QFileInfo(item).exists()) {
                image->attachVector(item);
                attachedCount++;
                modifiedIndices.append(i);
                break;
            }
        }
    }

    return attachedCount;
}

Commands::AddArtworksCommand::~AddArtworksCommand() {
    LOG_DEBUG << "#";
}

std::shared_ptr<Commands::ICommandResult> Commands::AddArtworksCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << m_FilePathes.length() << "images," << m_VectorsPathes.length() << "vectors";
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();

    const int newFilesCount = artworksRepository->getNewFilesCount(m_FilePathes);
    const int initialCount = artItemsModel->rowCount();
    bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_FilePathes);

    QVector<Models::ArtworkMetadata*> artworksToImport;
    artworksToImport.reserve(newFilesCount);

    if (newFilesCount > 0) {
        LOG_INFO << newFilesCount << "new files found";
        LOG_INFO << "Current files count is" << artItemsModel->getArtworksCount();
        artItemsModel->beginAccountingFiles(newFilesCount);

        int count = m_FilePathes.count();
        artworksToImport.reserve(count);

        for (int i = 0; i < count; ++i) {
            const QString &filename = m_FilePathes[i];

            if (artworksRepository->accountFile(filename)) {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename);
                commandManager->connectArtworkSignals(metadata);

                artItemsModel->appendMetadata(metadata);
                artworksToImport.append(metadata);

                LOG_INFO << "Added file:" << filename;

                const QString &dirPath = QFileInfo(filename).absolutePath();
                commandManager->addToRecentDirectories(dirPath);
            } else {
                LOG_INFO << "Rejected file:" << filename;
            }
        }

        artItemsModel->endAccountingFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);

    QHash<QString, QHash<QString, QString> > vectorsHash;
    decomposeVectors(vectorsHash);
    QVector<int> modifiedIndices;

    int attachedCount = artItemsModel->attachVectors(vectorsHash, modifiedIndices);

    if (m_AutoDetectVectors) {
        modifiedIndices.clear();
        attachedCount = findAndAttachVectors(artworksToImport, modifiedIndices);
    }

    if (newFilesCount > 0) {
        int length = artItemsModel->rowCount();
        int start = length - newFilesCount, end = length - 1;
        QVector<QPair<int, int> > ranges;
        ranges << qMakePair(start, end);
        commandManager->readMetadata(artworksToImport, ranges);

        artworksRepository->updateCountsForExistingDirectories();

        std::unique_ptr<UndoRedo::IHistoryItem> addArtworksItem(new UndoRedo::AddArtworksHistoryItem(initialCount, newFilesCount));
        commandManager->recordHistoryItem(addArtworksItem);
    } else if (attachedCount > 0) {
        artItemsModel->updateItems(modifiedIndices, QVector<int>() << Models::ArtItemsModel::HasVectorAttachedRole);
    }

    artItemsModel->raiseArtworksAdded(newFilesCount, attachedCount);

    std::shared_ptr<AddArtworksCommandResult> result(new AddArtworksCommandResult(newFilesCount));
    return result;
}

void Commands::AddArtworksCommand::decomposeVectors(QHash<QString, QHash<QString, QString> > &vectors) const {
    int size = m_VectorsPathes.size();
    LOG_DEBUG << size << "item(s)";

    for (int i = 0; i < size; ++i) {
        const QString &path = m_VectorsPathes.at(i);
        QFileInfo fi(path);
        const QString &absolutePath = fi.absolutePath();

        if (!vectors.contains(absolutePath)) {
            vectors.insert(absolutePath, QHash<QString, QString>());
        }

        vectors[absolutePath].insert(fi.baseName().toLower(), path);
    }
}
