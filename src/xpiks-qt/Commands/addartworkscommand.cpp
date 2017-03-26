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
        QStringList vectors = Helpers::convertToVectorFilenames(filepath);

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

void accountVectors(Models::ArtworksRepository *artworksRepository, const QVector<Models::ArtworkMetadata*> &artworks) {
    LOG_DEBUG << "#";

    int size = artworks.size();
    for (int i = 0; i < size; ++i) {
        Models::ArtworkMetadata *metadata = artworks.at(i);
        Models::ImageArtwork *imageArtwork = dynamic_cast<Models::ImageArtwork *>(metadata);
        if ((imageArtwork != nullptr) && imageArtwork->hasVectorAttached()) {
            artworksRepository->accountVector(imageArtwork->getAttachedVectorPath());
        }
    }
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
    const bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_FilePathes);

    QVector<Models::ArtworkMetadata*> artworksToImport;
    artworksToImport.reserve(newFilesCount);
    QStringList filesToWatch;
    filesToWatch.reserve(newFilesCount);

    if (newFilesCount > 0) {
        LOG_INFO << newFilesCount << "new files found";
        LOG_INFO << "Current files count is" << initialCount;
        artItemsModel->beginAccountingFiles(newFilesCount);

        const int count = m_FilePathes.count();

        for (int i = 0; i < count; ++i) {
            const QString &filename = m_FilePathes[i];
            qint64 directoryID = 0;

            if (artworksRepository->accountFile(filename, directoryID)) {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename, directoryID);
                commandManager->connectArtworkSignals(metadata);

                LOG_INTEGRATION_TESTS << "Added file:" << filename;

                artItemsModel->appendMetadata(metadata);
                artworksToImport.append(metadata);
                filesToWatch.append(filename);
            } else {
                LOG_INFO << "Rejected file:" << filename;
            }
        }

        artItemsModel->endAccountingFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);
    artworksRepository->watchFilePaths(filesToWatch);
    artworksRepository->updateFilesCounts();

    QHash<QString, QHash<QString, QString> > vectorsHash;
    decomposeVectors(vectorsHash);
    QVector<int> modifiedIndices;

    int attachedCount = artItemsModel->attachVectors(vectorsHash, modifiedIndices);

    if (m_AutoDetectVectors) {
        QVector<int> autoAttachedIndices;
        attachedCount = findAndAttachVectors(artworksToImport, autoAttachedIndices);

        foreach (int index, autoAttachedIndices) {
            modifiedIndices.append(initialCount + index);
        }
    }

    if (newFilesCount > 0) {
        int length = artItemsModel->rowCount();
        int start = length - newFilesCount, end = length - 1;
        QVector<QPair<int, int> > ranges;
        ranges << qMakePair(start, end);
        commandManager->readMetadata(artworksToImport, ranges);
        accountVectors(artworksRepository, artworksToImport);
        artworksRepository->updateCountsForExistingDirectories();

        std::unique_ptr<UndoRedo::IHistoryItem> addArtworksItem(new UndoRedo::AddArtworksHistoryItem(getCommandID() ,initialCount, newFilesCount));
        commandManager->recordHistoryItem(addArtworksItem);

        commandManager->addToRecentFiles(filesToWatch);
    }

    artItemsModel->raiseArtworksAdded(newFilesCount, attachedCount);
    artItemsModel->updateItems(modifiedIndices, QVector<int>() << Models::ArtItemsModel::HasVectorAttachedRole);

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
