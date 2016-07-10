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

#include "ftpuploaderworker.h"
#include <QSemaphore>
#include <QCoreApplication>
#include "curlftpuploader.h"
#include "../Models/uploadinfo.h"
#include "../Common/defines.h"
#include "uploadbatch.h"

namespace Conectivity {
    FtpUploaderWorker::FtpUploaderWorker(QSemaphore *uploadSemaphore,
                                         const std::shared_ptr<UploadBatch> &batch,
                                         const std::shared_ptr<Models::UploadInfo> &uploadInfo,
                                         QObject *parent) :
        QObject(parent),
        m_UploadSemaphore(uploadSemaphore),
        m_UploadBatch(batch),
        m_UploadInfo(uploadInfo)
    {
    }

    FtpUploaderWorker::~FtpUploaderWorker() {
    }

    void FtpUploaderWorker::process() {
        const QString &host = m_UploadBatch->getContext()->m_Host;

        LOG_DEBUG << "Waiting for the semaphore" << host;
        m_UploadSemaphore->acquire();

        LOG_INFO << "Starting upload to" << host;
        doUpload();

        m_UploadSemaphore->release();
        LOG_DEBUG << "Released semaphore" << host;

        emit stopped();
    }

    void FtpUploaderWorker::progressChangedHandler(double oldPercents, double newPercents) {
        Q_UNUSED(oldPercents);
        m_UploadInfo->setPercent(floor(newPercents));
    }

    void FtpUploaderWorker::doUpload() {
        CurlFtpUploader ftpUploader(m_UploadBatch);

        //QObject::connect(&ftpUploader, SIGNAL(uploadStarted()), this, SIGNAL(uploadStarted()));
        QObject::connect(&ftpUploader, SIGNAL(uploadFinished(bool)), this, SIGNAL(uploadFinished(bool)));
        QObject::connect(&ftpUploader, SIGNAL(progressChanged(double, double)), this, SIGNAL(progressChanged(double, double)));
        QObject::connect(&ftpUploader, SIGNAL(progressChanged(double, double)), this, SLOT(progressChangedHandler(double,double)));
        QObject::connect(this, SIGNAL(workerCancelled()), &ftpUploader, SLOT(cancel()));
        QObject::connect(&ftpUploader, SIGNAL(transferFailed(QString, QString)),
                         this, SIGNAL(transferFailed(QString, QString)));

        ftpUploader.uploadBatch();
        // in order to deliver 100% progressChanged() signal
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

