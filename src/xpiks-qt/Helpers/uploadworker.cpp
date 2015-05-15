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

#include "uploadworker.h"
#include "uploaditem.h"
#include <QStringList>
#include <QByteArray>
#include <QDebug>
#include "../Encryption/secretsmanager.h"

namespace Helpers {
    UploadWorker::UploadWorker(UploadItem *uploadItem, Encryption::SecretsManager *secretsManager, QObject *parent) :
        QObject(parent),
        m_UploadItem(uploadItem),
        m_SecretsManager(secretsManager)
    {
    }

    UploadWorker::~UploadWorker() { delete m_UploadItem; delete m_CurlProcess; }

    void UploadWorker::process() {
        m_SuccessStatus = false;

        QStringList *filesToUpload = m_UploadItem.m_FilesToUpload;
        Models::UploadInfo *uploadInfo = m_UploadItem->m_UploadInfo;

        const QString curlPath = uploadItem.m_CurlPath;
        int oneItemUploadMinutesTimeout = uploadItem.m_OneItemUploadMinutesTimeout;
        int maxSeconds = oneItemUploadMinutesTimeout * 60 * filesToUpload->length();

        QString password = m_SecretsManager->decodePassword(uploadInfo->getPassword());
        QString command = QString("%1 --connect-timeout 10 --max-time %6 --retry 1 -T \"{%2}\" %3 --user %4:%5").
                arg(curlPath, filesToUpload->join(','), uploadInfo->getHost(), uploadInfo->getUsername(), password, QString::number(maxSeconds));

        m_CurlProcess.start(command);

        // TODO: move to config
        if (m_CurlProcess.waitForFinished(maxSeconds * 1000) &&
                m_CurlProcess.exitStatus() == QProcess::NormalExit &&
                m_CurlProcess.exitCode() == 0) {
            m_SuccessStatus = true;
        }

        QByteArray stdoutByteArray = m_CurlProcess.readAllStandardOutput();
        QString stdoutText(stdoutByteArray);
        qDebug() << "STDOUT [" << uploadInfo->getHost() << "]:" << stdoutText;

        QByteArray stderrByteArray = m_CurlProcess.readAllStandardError();
        QString stderrText(stderrByteArray);
        qDebug() << "STDERR [" << uploadInfo->getHost() << "]:" << stderrText;

        // upload
        emit finished(m_SuccessStatus);
        emit stopped();
    }

    void UploadWorker::cancel() {
        m_CurlProcess.kill();
        emit finished(false);
        emit stopped();
    }
}
