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

#ifndef CURLWRAPPER
#define CURLWRAPPER

#include <QStringList>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include <QDebug>
#include <QPair>
#include "externaltoolsprovider.h"
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"
#include "../Encryption/secretsmanager.h"
#include "curlwrapper.h"

Helpers::UploadItem uploadViaCurl(Helpers::UploadItem uploadItem) {
    QStringList *filesToUpload = uploadItem.m_FilesToUpload;
    Models::UploadInfo *uploadInfo = uploadItem.m_UploadInfo;

    const QString curlPath = Helpers::ExternalToolsProvider::getCurlPath();
    // 10 minutes for each
    int maxSeconds = 10*60*filesToUpload->length();

    QString password = uploadItem.m_SecretsManager->decodePassword(uploadInfo->getPassword());
    QString command = QString("%1 --connect-timeout 10 --max-time %6 --retry 1 -T \"{%2}\" %3 --user %4:%5").
            arg(curlPath, filesToUpload->join(','), uploadInfo->getHost(), uploadInfo->getUsername(), password, QString::number(maxSeconds));

    QProcess process;
    process.start(command);
    // TODO: move to config
    if (process.waitForFinished(maxSeconds * 1000) &&
            process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0) {
        uploadItem.m_Success = true;
    }

    QByteArray stdoutByteArray = process.readAllStandardOutput();
    QString stdoutText(stdoutByteArray);
    qDebug() << "STDOUT: " << stdoutText;

    QByteArray stderrByteArray = process.readAllStandardError();
    QString stderrText(stderrByteArray);
    qDebug() << "STDERR: " << stderrText;

    return uploadItem;
}

bool isConnectionValid(const QString &host, const QString &username, const QString &password) {
    bool isValid = false;

    const QString curlPath = Helpers::ExternalToolsProvider::getCurlPath();

    QString command = QString("%1 %2 --user %3:%4").arg(curlPath, host, username, password);
    QProcess process;

    process.start(command);
    if (process.waitForFinished(7 * 1000) &&
            process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0) {
        isValid = true;
    }

    QByteArray stdoutByteArray = process.readAllStandardOutput();
    QString stdoutText(stdoutByteArray);
    qDebug() << "STDOUT: " << stdoutText;

    QByteArray stderrByteArray = process.readAllStandardError();
    QString stderrText(stderrByteArray);
    qDebug() << "STDERR: " << stderrText;

    return isValid;
}

#endif // CURLWRAPPER

