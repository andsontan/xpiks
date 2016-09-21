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
#include "../Models/settingsmodel.h"
#include "curlftpuploader.h"
#include "uploadcontext.h"
#include "ftpuploaderworker.h"
#include "../Common/defines.h"
#include "conectivityhelpers.h"

#include <curl/curl.h>

namespace Conectivity {
    FtpCoordinator::FtpCoordinator(int maxParallelUploads, QObject *parent) :
        QObject(parent),
        m_UploadSemaphore(maxParallelUploads),
        m_OverallProgress(0.0),
        m_FinishedWorkersCount(0),
        m_AllWorkersCount(0),
        m_AnyFailed(false)
    {
    }

    void FtpCoordinator::uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                                        std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos) {
        LOG_INFO << "Trying to upload" << artworksToUpload.size() <<
                   "file(s) to" << uploadInfos.size() << "host(s)";

        if (artworksToUpload.empty() || uploadInfos.empty()) {
            LOG_WARNING << "Nothing or nowhere to upload. Skipping...";
            return;
        }

        Encryption::SecretsManager *secretsManager = m_CommandManager->getSecretsManager();
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        std::vector<std::shared_ptr<UploadBatch> > batches = std::move(generateUploadBatches(artworksToUpload,
                                                                                             uploadInfos,
                                                                                             secretsManager,
                                                                                             settingsModel));

        Q_ASSERT(batches.size() == uploadInfos.size());

        size_t size = batches.size();

        initUpload(size);
        emit uploadStarted();

        for (size_t i = 0; i < size; ++i) {
            FtpUploaderWorker *worker = new FtpUploaderWorker(&m_UploadSemaphore,
                                                              batches.at(i), uploadInfos.at(i));
            QThread *thread = new QThread();
            worker->moveToThread(thread);
            QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

            QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
            QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            //QObject::connect(worker, SIGNAL(uploadStarted()), this, SIGNAL(uploadStarted()));
            QObject::connect(worker, SIGNAL(uploadFinished(bool)), this, SLOT(workerFinished(bool)));
            QObject::connect(this, SIGNAL(cancelAll()), worker, SIGNAL(workerCancelled()));
            QObject::connect(worker, SIGNAL(progressChanged(double,double)),
                             this, SLOT(workerProgressChanged(double,double)));
            QObject::connect(worker, SIGNAL(transferFailed(QString, QString)),
                             this, SIGNAL(transferFailed(QString, QString)));

            thread->start();
        }
    }

    void FtpCoordinator::cancelUpload() {
        LOG_DEBUG << "#";
        emit cancelAll();
    }

    void FtpCoordinator::workerProgressChanged(double oldPercents, double newPercents) {
        Q_ASSERT(m_AllWorkersCount > 0);
        double change = (newPercents - oldPercents) / m_AllWorkersCount;
        m_OverallProgress += change;
        emit overallProgressChanged(m_OverallProgress);
    }

    void FtpCoordinator::workerFinished(bool anyErrors) {
        LOG_DEBUG << "anyErrors =" << anyErrors;

        if (anyErrors) {
            m_AnyFailed = true;
        }

        int workersDone = m_FinishedWorkersCount.fetchAndAddOrdered(1) + 1;

        if ((size_t)workersDone == m_AllWorkersCount) {
            finalizeUpload();
            emit uploadFinished(m_AnyFailed);
            emit overallProgressChanged(100.0);
        }
    }

    void FtpCoordinator::initUpload(size_t uploadBatchesCount) {
        m_AnyFailed = false;
        m_AllWorkersCount = uploadBatchesCount;
        m_FinishedWorkersCount = 0;
        m_OverallProgress = 0.0;

        // should be called in main() using initHelper
        //curl_global_init(CURL_GLOBAL_ALL);
    }

    void FtpCoordinator::finalizeUpload() {
        Q_ASSERT((size_t)m_FinishedWorkersCount == m_AllWorkersCount);
        // should be called in main() using initHelper
        // curl_global_cleanup();
    }
}
