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

#include "metadatareadingworker.h"
#include <QJsonDocument>
#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include "backupsaverservice.h"
#include "../Models/artworkmetadata.h"

namespace MetadataIO {
    MetadataReadingWorker::MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                                 BackupSaverService *saverService,
                                                 const QString &exiftoolPath):
        m_ItemsToRead(itemsToRead),
        m_BackupSaverService(saverService),
        m_ExiftoolPath(exiftoolPath),
        m_ExiftoolProcess(NULL)
    {
    }

    MetadataReadingWorker::~MetadataReadingWorker() {
        if (m_ExiftoolProcess != NULL) {
            delete m_ExiftoolProcess;
        }
    }

    void MetadataReadingWorker::process() {
        initWorker();

        QTemporaryFile file;

        if (file.open()) {
            QTextStream out(&file);
            QStringList exiftoolArguments = createArgumentsList();
            foreach (const QString &line, exiftoolArguments) {
                out << line << std::endl;
            }

            QString exiftoolCommand = QString("%1 -@ %2").arg(m_ExiftoolPath).arg(file.fileName());
            qDebug() << "Starting exiftool with command:" << exiftoolCommand;
            m_ExiftoolProcess->start(exiftoolCommand);


        }

        if (m_ExiftoolProcess->exitStatus() == QProcess::NormalExit) {
            QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
            QJsonDocument document = QJsonDocument::fromJson(stdoutByteArray);
        }
        m_BackupSaverService->readArtworks(m_ItemsToRead);
    }

    void MetadataReadingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        bool success = exitCode == 0 && exitStatus == QProcess::NormalExit;
        emit finished(success);
    }

    void MetadataReadingWorker::initWorker() {
        m_ExiftoolProcess = new QProcess();

        QObject::connect(m_ExiftoolProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(innerProcessFinished(int,QProcess::ExitStatus)));
    }

    QStringList MetadataReadingWorker::createArgumentsList() {
        QStringList arguments;
        arguments.reserve(m_ItemsToRead.length() + 10);

        arguments << "-json" << "-ignoreMinorErrors" << "-e";
        arguments << "-ObjectName" << "-Title";
        arguments << "-ImageDescription" << "-Description" << "-Caption-Abstract";
        arguments << "-Keywords" << "-Subject";

        int size = m_ItemsToRead.length();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToRead.at(i);
            arguments << metadata->getFilepath();
        }

        return arguments;
    }
}

