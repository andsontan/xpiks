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
#include <QPair>
#include "externaltoolsprovider.h"
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"

typedef QPair<QStringList*, Models::UploadInfo*> UploadPair;

UploadPair uploadViaCurl(UploadPair pair) {
    QStringList *filesToUpload = pair.first;
    Models::UploadInfo *uploadInfo = pair.second;

    const QString curlPath = Helpers::ExternalToolsProvider::getCurlPath();

    QStringList arguments;
    arguments << "--retry 1";
    arguments << QString("-T {%1}").arg(filesToUpload->join(','));
    arguments << uploadInfo->getHost() << "--user" << QString("%1:%2").arg(uploadInfo->getUsername(), uploadInfo->getPassword());

    Models::UploadInfo *resultInfo = NULL;
    QProcess process;
    process.start(curlPath, arguments);
    // wait each for 10 minutes
    // TODO: move to config
    if (process.waitForFinished(600000) &&
            process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0) {
        resultInfo = uploadInfo;
    }

    return qMakePair(filesToUpload, resultInfo);
}

#endif // CURLWRAPPER

