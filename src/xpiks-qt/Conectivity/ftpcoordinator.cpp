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

#include "ftpcoordinator.h"
#include <QStringList>
#include <QSharedData>
#include "../Models/artworkmetadata.h"
#include "../Models/uploadinfo.h"
#include "../Helpers/filenameshelpers.h"
#include "curlftpuploader.h"
#include "uploadcontext.h"

#define TIMEOUT_SECONDS 10
#define RETRIES_COUNT 3

namespace Conectivity {
    void extractFilePathes(const QVector<Models::ArtworkMetadata *> &artworkList,
                           QStringList &filePathes,
                           QStringList &zipsPathes,
                           bool includeVector) const {

        int size = artworkList.length();
        qDebug() << "Generating filepathes for upload of" << size << "item(s)";

        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = artworkList.at(i);
            QString filepath = metadata->getFilepath();
            filePathes.append(filepath);

            if (includeVector) {
                QStringList vectors = Helpers::convertToVectorFilenames(QStringList() << filepath);

                foreach (const QString &item, vectors) {
                    if (QFileInfo(item).exists()) {
                        filePathes.append(item);
                        break;
                    }
                }

                QString zipPath = Helpers::getArchivePath(filepath);
                zipsPathes.append(zipPath);
            }
        }
    }

    void generateUploadContexts(const QVector<Models::UploadInfo *> &uploadInfos,
                                QVector<QSharedPointer<UploadContext> > &contexts) {
        int size = uploadInfos.size();
        contexts.reserve(size);

        for (int i = 0; i < size; ++i) {
            UploadContext *context = new UploadContext();
            Models::UploadInfo *info = uploadInfos.at(i);

            context->m_Host = info->getHost();
            context->m_Username = info->getUsername();
            context->m_Password = info->getPassword();
            context->m_UsePassiveMode = info->getFtpPassiveMode();

            // TODO: move to configs/options
            context->m_TimeoutSeconds = TIMEOUT_SECONDS;
            context->m_RetriesCount = RETRIES_COUNT;

            contexts.append(QSharedPointer(context));
        }
    }

    QVector<UploadBatch> generateUploadBatches(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                               const QVector<Models::UploadInfo *> &uploadInfos,
                               bool includeVector) {
        QStringList filePathes;
        QStringList zipFilePathes;
        extractFilePathes(artworksToUpload, filePathes, zipFilePathes, includeVector);

        QVector<QSharedDataPointer<UploadContext> > contexts;
        generateUploadContexts(uploadInfos, contexts);

        int size = artworksToUpload.size();
        for (int i = 0; i < size; ++i) {
            // TODO:
        }
    }

    FtpCoordinator::FtpCoordinator(int maxParallelUploads, QObject *parent) :
        QObject(parent),
        m_UploadSemaphore(maxParallelUploads),
        m_MaxParallelUploads(maxParallelUploads)
    {
    }
}

