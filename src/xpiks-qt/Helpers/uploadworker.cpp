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

#include "uploadworker.h"
#include "uploaditem.h"
#include <QStringList>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include "../Encryption/secretsmanager.h"
#include "../Helpers/ziphelper.h"

namespace Helpers {
    UploadWorker::UploadWorker(UploadItem *uploadItem, const Encryption::SecretsManager *secretsManager,
                               int delay, QObject *parent) :
        QObject(parent),
        m_UploadItem(uploadItem),
        m_SecretsManager(secretsManager),
        m_CurlProcess(NULL),
        m_Timer(NULL),
        m_Host(uploadItem->m_UploadInfo->getHost()),
        m_PercentRegexp("[^0-9.]"),
        m_Delay(delay),
        m_PercentDone(0.0),
        m_FilesUploaded(0)
    {
    }

    UploadWorker::~UploadWorker() {
        delete m_UploadItem;

        if (m_CurlProcess) {
            delete m_CurlProcess;
        }

        if (m_Timer) {
            delete m_Timer;
        }
    }

    void UploadWorker::process() {

        QThread::sleep(m_Delay);

        const QStringList &filesToUpload = m_UploadItem->m_FilesToUpload;
        Models::UploadInfo *uploadInfo = m_UploadItem->m_UploadInfo;
        m_OverallFilesCount = filesToUpload.length();

        const QString curlPath = m_UploadItem->m_CurlPath;
        int oneItemUploadMinutesTimeout = m_UploadItem->m_OneItemUploadMinutesTimeout;
        int maxSeconds = oneItemUploadMinutesTimeout * 60 * filesToUpload.length();

        QString password = m_SecretsManager->decodePassword(uploadInfo->getPassword());
        QString command = QString("%1 --progress-bar --connect-timeout 10 --max-time %6 --retry 1 -T \"{%2}\" %3 --user %4:%5").
                arg(curlPath, filesToUpload.join(','), uploadInfo->getHost(), uploadInfo->getUsername(), password, QString::number(maxSeconds));

        m_CurlProcess = new QProcess();

        QObject::connect(m_CurlProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(innerProcessFinished(int,QProcess::ExitStatus)));
        QObject::connect(m_CurlProcess, SIGNAL(readyReadStandardError()),
                         this, SLOT(uploadOutputReady()));

        m_Timer = new QTimer();
        QObject::connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
        m_Timer->setSingleShot(true);

        m_Timer->start(maxSeconds*1000);
        m_CurlProcess->start(command);
    }

    void UploadWorker::cancel() {
        qDebug() << "Terminating upload to " << m_Host;

        bool killed = false;

        if (m_CurlProcess && m_CurlProcess->state() != QProcess::NotRunning && !m_CurlProcess->atEnd()) {
            m_CurlProcess->kill();
            qDebug() << "Curl process killed";
            killed = true;
        }

        emit finished(!killed);
        emit stopped();
    }

    void UploadWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        QByteArray stdoutByteArray = m_CurlProcess->readAllStandardOutput();
        QString stdoutText(stdoutByteArray);
        qDebug() << "STDOUT [" << m_Host << "]:" << stdoutText;

        QByteArray stderrByteArray = m_CurlProcess->readAllStandardError();
        QString stderrText(stderrByteArray);
        qDebug() << "STDERR [" << m_Host << "]:" << stderrText;

        // upload
        bool success = exitCode == 0 && exitStatus == QProcess::NormalExit;
        emitFinishSignals(success);
    }

    void UploadWorker::uploadOutputReady()
    {
        QString output = m_CurlProcess->readAllStandardError();
        QString prettyfiedOutput = output.right(10).trimmed();

        double percent = parsePercent(prettyfiedOutput);
        bool anotherFileUploaded = qAbs(percent - 100.0) < 0.000000001;

        percent += m_FilesUploaded*100.0;
        percent /= (m_OverallFilesCount + 0.0);

        if (anotherFileUploaded) { m_FilesUploaded++; }

        if (percent > m_PercentDone) {
            percentChanged(percent, m_PercentDone);
            m_PercentDone = percent;
        }
    }

    void UploadWorker::onTimerTimeout()
    {
        cancel();
    }

    double UploadWorker::parsePercent(QString &curlOutput) const
    {
        double value = 0.0;
        if (curlOutput.endsWith("%")) {
            curlOutput.remove(m_PercentRegexp);
            value = curlOutput.toDouble();
        }

        if (value < 0 || value > 100.0) {
            value = 0.0;
        }

        return value;
    }

    void UploadWorker::emitFinishSignals(bool success) {
        emit finished(success);
        emit stopped();
    }
}
