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

#include <QDebug>
#include <QFileInfo>
#include <QVector>
#include <QHash>
#include "addartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artworkmetadata.h"
#include "../Models/artitemsmodel.h"
#include "../UndoRedo/addartworksitem.h"
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"
#include "../Helpers/filenameshelpers.h"

int findAndAttachVectors(const QVector<Models::ArtworkMetadata*> &artworksList) {
    int attachedCount = 0;

    int size = artworksList.length();
    for (int i = 0; i < size; ++i) {
        Models::ArtworkMetadata *metadata = artworksList.at(i);
        if (metadata->hasVectorAttached()) {
            attachedCount++;
            continue;
        }

        const QString &filepath = metadata->getFilepath();
        QStringList vectors = Helpers::convertToVectorFilenames(QStringList() << filepath);

        foreach (const QString &item, vectors) {
            if (QFileInfo(item).exists()) {
                metadata->attachVector(item);
                attachedCount++;
                break;
            }
        }
    }

    return attachedCount;
}

Commands::CommandResult *Commands::AddArtworksCommand::execute(const ICommandManager *commandManagerInterface) const {
    qDebug() << "AddArtworksCommand::execute #" << m_FilePathes.length() << "files received";
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();

    const int newFilesCount = artworksRepository->getNewFilesCount(m_FilePathes);
    const int initialCount = artItemsModel->rowCount();
    bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_FilePathes);

    QVector<Models::ArtworkMetadata*> artworksToImport;
    artworksToImport.reserve(newFilesCount);

    if (newFilesCount > 0) {
        qInfo() << "AddArtworksCommand::execute #" << newFilesCount << "new files found";
        artItemsModel->beginAccountingFiles(newFilesCount);

        int count = m_FilePathes.count();
        artworksToImport.reserve(count);

        for (int i = 0; i < count; ++i) {
            const QString &filename = m_FilePathes[i];

            if (artworksRepository->accountFile(filename))
            {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename);
                commandManager->connectArtworkSignals(metadata);

                artItemsModel->appendMetadata(metadata);
                artworksToImport.append(metadata);

                const QString &dirPath = QFileInfo(filename).absolutePath();
                commandManager->addToRecentDirectories(dirPath);
            }
        }

        artItemsModel->endAccountingFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);

    QHash<QString, QPair<QString, QString> > vectorsHash;
    decomposeVectors(vectorsHash);
    int attachedCount = artItemsModel->attachVectors(vectorsHash);

    Models::SettingsModel *settingsModel = commandManager->getSettingsModel();
    if (settingsModel->getAutoFindVectors()) {
        attachedCount = findAndAttachVectors(artworksToImport);
    }

    if (newFilesCount > 0) {
        int length = artItemsModel->rowCount();
        int start = length - newFilesCount, end = length - 1;
        QVector<QPair<int, int> > ranges;
        ranges << qMakePair(start, end);
        commandManager->readMetadata(artworksToImport, ranges);

        artworksRepository->updateCountsForExistingDirectories();

        UndoRedo::AddArtworksHistoryItem *addArtworksItem = new UndoRedo::AddArtworksHistoryItem(initialCount, newFilesCount);
        commandManager->recordHistoryItem(addArtworksItem);
    }

    if ((newFilesCount > 0) || (attachedCount > 0)) {
        artItemsModel->raiseArtworksAdded(newFilesCount, attachedCount);
    }

    AddArtworksCommandResult *result = new AddArtworksCommandResult(newFilesCount);
    return result;
}

void Commands::AddArtworksCommand::decomposeVectors(QHash<QString, QPair<QString, QString> > &vectors) const {
    qDebug() << "AddArtworksCommand::decomposeVectors #";

    int size = m_VectorsPathes.size();
    for (int i = 0; i < size; ++i) {
        const QString &path = m_VectorsPathes.at(i);
        QFileInfo fi(path);
        vectors.insert(fi.absolutePath(), qMakePair(fi.baseName(), path));
    }
}
