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
#include <QThread>
#include "../Models/artworkmetadata.h"
#include "../Models/uploadinfo.h"
#include "../Helpers/filenameshelpers.h"
#include "../Encryption/secretsmanager.h"
#include "../Commands/commandmanager.h"
#include "curlftpuploader.h"
#include "uploadcontext.h"
#include "ftpuploaderworker.h"

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
            }

            // TODO: maybe there's a sense zipping only vectors?
            QString zipPath = Helpers::getArchivePath(filepath);
            zipsPathes.append(zipPath);
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

    QVector<UploadBatch*> generateUploadBatches(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                               const QVector<Models::UploadInfo *> &uploadInfos,
                               bool includeVector) {
        QVector<UploadBatch*> batches;

        QStringList filePathes;
        QStringList zipFilePathes;
        extractFilePathes(artworksToUpload, filePathes, zipFilePathes, includeVector);

        QVector<QSharedDataPointer<UploadContext> > contexts;
        generateUploadContexts(uploadInfos, contexts);

        int size = contexts.size();
        batches.reserve(size);

        for (int i = 0; i < size; ++i) {
            UploadBatch *batch;
            const QSharedDataPointer<UploadContext> &context = contexts.at(i);

            if (uploadInfos[i]->getZipBeforeUpload()) {
                batch = new UploadBatch(context, zipFilePathes);
            } else {
                batch = new UploadBatch(context, filePathes);
            }

            batches.append(batch);
        }

        return batches;
    }

    FtpCoordinator::FtpCoordinator(int maxParallelUploads, QObject *parent) :
        QObject(parent),
        m_UploadSemaphore(new QSemaphore(maxParallelUploads)),
        m_MaxParallelUploads(maxParallelUploads)
    {
    }

    void FtpCoordinator::uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                                        const QVector<Models::UploadInfo *> &uploadInfos,
                                        bool includeVectors) {
        QVector<UploadBatch*> batches = generateUploadBatches(artworksToUpload, uploadInfos, includeVectors);
        Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();

        int size = batches.size();
        for (int i = 0; i < size; ++i) {
            FtpUploaderWorker *worker = new FtpUploaderWorker(&m_UploadSemaphore, secretsManager, batches.at(i));
            QThread *thread = new QThread();
            worker->moveToThread(thread);

            QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

            QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
            QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            QObject::connect(worker, SIGNAL(uploadFinished(bool)), this, SLOT(workerFinished(bool)));
            QObject::connect(this, SIGNAL(cancelAll()), worker, SLOT(cancel()));

            thread->start();
        }
    }
}

