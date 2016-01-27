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

#include "../../libcurl/include/curl/curl.h"

#define TIMEOUT_SECONDS 10
#define RETRIES_COUNT 3

namespace Conectivity {
    void extractFilePathes(const QVector<Models::ArtworkMetadata *> &artworkList,
                           QStringList &filePathes,
                           QStringList &zipsPathes,
                           bool includeVector) {

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
                                QVector<QSharedPointer<UploadContext> > &contexts,
                                Encryption::SecretsManager *secretsManager) {
        int size = uploadInfos.size();
        contexts.reserve(size);

        for (int i = 0; i < size; ++i) {
            UploadContext *context = new UploadContext();
            Models::UploadInfo *info = uploadInfos.at(i);

            context->m_Host = info->getHost();
            context->m_Username = info->getUsername();
            context->m_Password = secretsManager->decodePassword(info->getPassword());
            context->m_UsePassiveMode = info->getFtpPassiveMode();

            // TODO: move to configs/options
            context->m_TimeoutSeconds = TIMEOUT_SECONDS;
            context->m_RetriesCount = RETRIES_COUNT;

            contexts.append(QSharedPointer<UploadContext>(context));
        }
    }

    QVector<UploadBatch*> generateUploadBatches(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                                                const QVector<Models::UploadInfo *> &uploadInfos,
                                                Encryption::SecretsManager *secretsManager,
                                                bool includeVector) {
        QVector<UploadBatch*> batches;

        QStringList filePathes;
        QStringList zipFilePathes;
        extractFilePathes(artworksToUpload, filePathes, zipFilePathes, includeVector);

        QVector<QSharedPointer<UploadContext> > contexts;
        generateUploadContexts(uploadInfos, contexts, secretsManager);

        int size = contexts.size();
        batches.reserve(size);

        for (int i = 0; i < size; ++i) {
            UploadBatch *batch;
            const QSharedPointer<UploadContext> &context = contexts.at(i);

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
        m_UploadSemaphore(maxParallelUploads),
        m_MaxParallelUploads(maxParallelUploads),
        m_OverallProgress(0.0),
        m_FinishedWorkersCount(0),
        m_AnyFailed(false)
    {
    }

    void FtpCoordinator::uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                                        const QVector<Models::UploadInfo *> &uploadInfos,
                                        bool includeVectors) {

        if (artworksToUpload.isEmpty() || uploadInfos.isEmpty()) {
            qWarning() << "Trying to upload" << artworksToUpload.size() << "files to" << uploadInfos.size() << "hosts";
            return;
        }

        Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();
        QVector<UploadBatch*> batches = generateUploadBatches(artworksToUpload,
                                                              uploadInfos,
                                                              secretsManager,
                                                              includeVectors);

        Q_ASSERT(batches.size() == uploadInfos.size());

        int size = batches.size();

        initUpload(size);

        for (int i = 0; i < size; ++i) {
            FtpUploaderWorker *worker = new FtpUploaderWorker(&m_UploadSemaphore,
                                                              batches.at(i), uploadInfos.at(i));
            QThread *thread = new QThread();
            worker->moveToThread(thread);
            QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

            QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
            QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            QObject::connect(worker, SIGNAL(uploadStarted()), this, SIGNAL(uploadStarted()));
            QObject::connect(worker, SIGNAL(uploadFinished(bool)), this, SLOT(workerFinished(bool)));
            QObject::connect(this, SIGNAL(cancelAll()), worker, SIGNAL(workerCancelled()));
            QObject::connect(worker, SIGNAL(progressChanged(double,double)),
                             this, SLOT(workerProgressChanged(double,double)));

            thread->start();
        }
    }

    void FtpCoordinator::cancelUpload() {
        qDebug() << "Cancelling upload in coordinator";
        emit cancelAll();
    }

    void FtpCoordinator::workerProgressChanged(double oldPercents, double newPercents) {
        Q_ASSERT(m_AllWorkersCount > 0);
        double change = (newPercents - oldPercents) / m_AllWorkersCount;
        m_OverallProgress += change;
        emit overallProgressChanged(m_OverallProgress);
    }

    void FtpCoordinator::workerFinished(bool anyErrors) {
        if (anyErrors) {
            m_AnyFailed = true;
        }

        int workersDone = m_FinishedWorkersCount.fetchAndAddOrdered(1) + 1;

        if (workersDone == m_AllWorkersCount) {
            finalizeUpload();
            emit uploadFinished(m_AnyFailed);
            emit overallProgressChanged(100.0);
        }
    }

    void FtpCoordinator::initUpload(int uploadBatchesCount) {
        m_AnyFailed = false;
        m_AllWorkersCount = uploadBatchesCount;
        m_FinishedWorkersCount = 0;
        m_OverallProgress = 0.0;
        qDebug() << "Initializing CURL";

        curl_global_init(CURL_GLOBAL_ALL);
    }

    void FtpCoordinator::finalizeUpload() {
        Q_ASSERT(m_FinishedWorkersCount == m_AllWorkersCount);
        curl_global_cleanup();
        qDebug() << "Cleaning up CURL";
    }
}
