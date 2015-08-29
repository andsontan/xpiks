/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#include "uploadcoordinator.h"
#include <QString>
#include <QRegExp>
#include <QFileInfo>
#include <QThread>
#include <QtMath>
#include "../Models/artworkmetadata.h"
#include "uploaditem.h"
#include "externaltoolsprovider.h"
#include "uploadworker.h"
#include "../Helpers/ziphelper.h"

namespace Helpers {
    void UploadCoordinator::uploadArtworks(const QList<Models::ArtworkMetadata *> &artworkList,
                                           const QList<Models::UploadInfo *> &uploadInfos,
                                           bool includeEPS, const Encryption::SecretsManager *secretsManager)
    {
        QStringList filePathes;
        QStringList zipsPathes;
        extractFilePathes(artworkList, filePathes, zipsPathes, includeEPS);

        QList<UploadItem*> uploadItems;
        const QString &curlPath = Helpers::ExternalToolsProvider::getCurlPath();
        int oneItemUploadTimeout = Helpers::ExternalToolsProvider::getOneItemUploadMinutesTimeout();

        int count = uploadInfos.length();
        for (int i = 0; i < count; ++i) {
            Models::UploadInfo * info = uploadInfos.at(i);
            if (info->getIsSelected()) {
                UploadItem *item;
                bool useZips = info->getZipBeforeUpload();
                item = new UploadItem(info, i, useZips ? zipsPathes : filePathes, oneItemUploadTimeout, curlPath);
                uploadItems.append(item);
            }
        }

        emit uploadStarted();

        m_AnyFailed = false;
        m_WorkersCount = 0;
        m_AllWorkersCount = uploadItems.count();
        m_PercentDone = 0;

        if (m_AllWorkersCount > 0) {
            doRunUpload(uploadItems, secretsManager);
        } else {
            emit uploadFinished(true);
        }
    }

    void UploadCoordinator::cancelUpload()
    {
        emit cancelAll();
    }

    void UploadCoordinator::workerFinished(int index, bool success)
    {
        bool allWorkersFinished = false;

        m_Mutex.lock();
        {
            m_AnyFailed = m_AnyFailed || !success;
            m_WorkersCount++;
            allWorkersFinished = m_WorkersCount == m_AllWorkersCount;
        }
        m_Mutex.unlock();

        emit itemFinished(success);

        if (allWorkersFinished) {
            Q_ASSERT(m_UploadSemaphore.available() == MAX_PARALLEL_UPLOAD);
            emit percentChanged(100);
            emit percentChangedForItem(index, 100);
            m_PercentDone = 100;
            emit uploadFinished(!m_AnyFailed);
            stopThreads();
        }
    }

    void UploadCoordinator::percentReported(int index, double newPercent, double oldPercent)
    {
        double n = m_UploadThreads.length();
        if (n == 0) { n = 1.0; }
        int percentDone = 0;

        m_PercentMutex.lock();
        {
            m_PercentDone -= oldPercent/n;
            m_PercentDone += newPercent/n;
            percentDone = m_PercentDone;
        }
        m_PercentMutex.unlock();

        emit percentChanged(percentDone);
        emit percentChangedForItem(index, (int)qFloor(newPercent));
    }

    void UploadCoordinator::doRunUpload(const QList<UploadItem*> &uploadItems,
                                        const Encryption::SecretsManager *secretsManager)
    {
        int i = 0;
        foreach (UploadItem *uploadItem, uploadItems) {
            UploadWorker *worker = new UploadWorker(uploadItem, secretsManager, &m_UploadSemaphore, i);
            QThread *thread = new QThread();
            worker->moveToThread(thread);

            QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

            QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
            QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            QObject::connect(worker, SIGNAL(finished(int,bool)), this, SLOT(workerFinished(int,bool)));
            QObject::connect(this, SIGNAL(cancelAll()), worker, SLOT(cancel()));

            QObject::connect(worker, SIGNAL(percentChanged(int, double,double)),
                             this, SLOT(percentReported(int, double,double)));

            m_UploadThreads.append(thread);

            thread->start();
            i++;
        }
    }

    void UploadCoordinator::extractFilePathes(const QList<Models::ArtworkMetadata *> &artworkList,
                                              QStringList &filePathes,
                                              QStringList &zipsPathes,
                                              bool includeEPS) const {

        foreach(Models::ArtworkMetadata *metadata, artworkList) {
            QString filepath = metadata->getFilepath();
            filePathes.append(filepath);

            if (includeEPS) {
                QString epsFilepath = filepath.replace(QRegExp("(.*)[.]jpg", Qt::CaseInsensitive), "\\1.eps");
                QString aiFilepath = filepath.replace(QRegExp("(.*)[.]jpg", Qt::CaseInsensitive), "\\1.ai");
                if (QFileInfo(epsFilepath).exists()) {
                    filePathes.append(epsFilepath);
                } else if (QFileInfo(aiFilepath).exists()) {
                    filePathes.append(aiFilepath);
                }
            }

            QString zipPath = Helpers::getArchivePath(filepath);
            zipsPathes.append(zipPath);
        }
    }

    void UploadCoordinator::stopThreads()
    {
        // threads are stopped on worker stopped signal
        // and deleted with thread finished signal binding
        m_UploadThreads.clear();
    }
}

