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

#include "uploadcoordinator.h"
#include <QString>
#include <QRegExp>
#include <QFileInfo>
#include <QThread>
#include "../Models/artworkmetadata.h"
#include "uploaditem.h"
#include "externaltoolsprovider.h"
#include "uploadworker.h"

namespace Helpers {
    void UploadCoordinator::uploadArtworks(const QList<Models::ArtworkMetadata *> &artworkList,
                                           const QList<UploadInfo *> &uploadInfos,
                                           bool includeEPS) const
    {
        m_FilePathes = new QStringList();
        extractFilePathes(artworkList, filePathes, includeEPS);

        QList<UploadItem*> uploadItems;
        const QString &curlPath = Helpers::ExternalToolsProvider::getCurlPath();
        int oneItemUploadTimeout = Helpers::ExternalToolsProvider::getOneItemUploadMinutesTimeout();

        emit uploadStarted();

        foreach (UploadInfo *info, uploadInfos) {
            if (info->getIsSelected()) {
                uploadItems.append(new UploadItem(info, m_FilePathes,
                                                  oneItemUploadTimeout, curlPath));
            }
        }

        emit uploadStarted();

        m_AnyFailed = false;
        m_WorkersCount = 0;
        m_AllWorkersCount = uploadItems.count();

        if (m_AllWorkersCount > 0) {
            doRunUpload(uploadItems);
        } else {
            emit uploadFinished(true);
        }
    }

    void UploadCoordinator::workerFinished(bool success)
    {
        bool allWorkersFinished = false;

        m_Mutex.lock();
        {
            m_AnyFailed = m_AnyFailed || !success;
            m_WorkersCount++;
            allWorkersFinished = m_WorkersCount == m_AllWorkersCount;
        }
        m_Mutex.unlock();

        if (allWorkersFinished) {
            emit uploadFinished(!m_AnyFailed);
        }
    }

    void UploadCoordinator::doRunUpload(const QList<UploadItem> &uploadItems)
    {
        foreach (UploadItem *uploadItem, uploadItems) {
            UploadWorker *worker = new UploadWorker(uploadItem);
            QThread *thread = new QThread();
            worker->moveToThread(thread);

            QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
            QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

            QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
            QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

            QObject::connect(worker, SIGNAL(finished(bool)), this, SLOT(workerFinished(bool));
            QObject::connect(this, SIGNAL(cancelAll()), worker, SLOT(cancel()));

            thread->start();
        }
    }

    void UploadCoordinator::extractFilePathes(const QList<Models::ArtworkMetadata *> &artworkList,
                                              QStringList *filePathes,
                                              bool includeEPS) {

        foreach(Models::ArtworkMetadata *metadata, artworkList) {
            QString filepath = metadata->getFilepath();
            filePathes->append(filepath);

            if (includeEPS) {
                QString epsFilepath = filepath.replace(QRegExp("(.*)[.]jpg", Qt::CaseInsensitive), "\\1.eps");
                if (QFileInfo(epsFilepath).exists()) {
                    filePathes->append(epsFilepath);
                }
            }
        }
    }
}

