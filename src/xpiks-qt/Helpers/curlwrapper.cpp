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

#include "curlwrapper.h"
#include <QStringList>
#include <QProcess>
#include <QRegExp>
#include <QDebug>
#include <QPair>
#include "../Models/uploadinfo.h"
#include "../Models/artworkmetadata.h"
#include "../Encryption/secretsmanager.h"
#include "../Helpers/testconnectionresult.h"
#include "curlwrapper.h"
#include "uploaditem.h"

Helpers::TestConnectionResult isConnectionValid(const QString &hostname, const QString &username,
                                                const QString &password, const QString &curlPath) {
    qDebug() << "Checking credentials to" << hostname;
    bool isValid = false;

    QString host = hostname;
    if (!host.startsWith("ftp://") && !host.startsWith("ftp.")) {
        host = QLatin1String("ftp://") + host;
    }

    QString command = QString("%1 %2 --user %3:%4").arg(curlPath, host, username, password);
    QProcess process;

    process.start(command);
    bool finishedInTime = process.waitForFinished(10 * 1000);
    if (finishedInTime && process.exitStatus() == QProcess::NormalExit &&
            process.exitCode() == 0) {
        isValid = true;
    } else {
        qWarning() << "Timeout or error while waiting for curl";
        qWarning() << "Error:" << curlPath << process.errorString();
    }

    QByteArray stdoutByteArray = process.readAllStandardOutput();
    QString stdoutText(stdoutByteArray);
    qDebug() << "STDOUT: " << stdoutText;

    QByteArray stderrByteArray = process.readAllStandardError();
    QString stderrText(stderrByteArray);
    qDebug() << "STDERR: " << stderrText;

    Helpers::TestConnectionResult result(isValid, host);
    return result;
}
