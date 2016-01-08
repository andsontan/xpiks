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
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDataStream>
#include "../Models/settingsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/constants.h"
#include "saverworkerjobitem.h"

#define SOURCEFILE QLatin1String("SourceFile")
#define TITLE QLatin1String("Title")
#define DESCRIPTION QLatin1String("Description")
#define OBJECTNAME QLatin1String("ObjectName")
#define IMAGEDESCRIPTION QLatin1String("ImageDescription")
#define CAPTIONABSTRACT QLatin1String("Caption-Abstract")
#define KEYWORDS QLatin1String("Keywords")
#define SUBJECT QLatin1String("Subject")

namespace MetadataIO {
    void parseJsonKeywords(const QJsonArray &array, ImportDataResult &result) {
        int size = array.size();
        QStringList keywords;
        keywords.reserve(size);

        for (int i = 0; i < size; ++i) {
            keywords.append(array.at(i).toString());
        }

        result.Keywords = keywords;
    }

    void jsonObjectToImportResult(const QJsonObject &object, ImportDataResult &result) {
        if (object.contains(SOURCEFILE)) {
            result.FilePath = object.value(SOURCEFILE).toString();
        }

        if (object.contains(TITLE)) {
            result.Title = object.value(TITLE).toString();
        } else if (object.contains(OBJECTNAME)) {
            result.Title = object.value(OBJECTNAME).toString();
        }

        if (object.contains(DESCRIPTION)) {
            result.Description = object.value(DESCRIPTION).toString();
        } else if (object.contains(CAPTIONABSTRACT)) {
            result.Description = object.value(CAPTIONABSTRACT).toString();
        } else if (object.contains(IMAGEDESCRIPTION)) {
            result.Description = object.value(IMAGEDESCRIPTION).toString();
        }

        bool keywordsSet = false;

        if (object.contains(KEYWORDS)) {
            QJsonValue keywords = object.value(KEYWORDS);
            if (keywords.isArray()) {
                parseJsonKeywords(keywords.toArray(), result);
                keywordsSet = true;
            } else {
                qWarning() << "Keywords object in json is not array";
                QString keywordsStr = object.value(KEYWORDS).toString();
                if (!keywordsStr.simplified().isEmpty()) {
                    // old Xpiks bug when it called exiftool with wrong arguments
                    if (keywordsStr.startsWith(QChar('"')) && keywordsStr.endsWith('"')) {
                        keywordsStr = keywordsStr.mid(1, keywordsStr.length() - 2);
                    }

                    result.Keywords = keywordsStr.split(QChar(','), QString::SkipEmptyParts);
                    keywordsSet = true;
                }
            }
        }

        if (!keywordsSet && object.contains(SUBJECT)) {
            result.Keywords = object.value(SUBJECT).toString().split(QChar(','), QString::SkipEmptyParts);
        }
    }

    MetadataReadingWorker::MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                                 Models::SettingsModel *settingsModel):
        m_ItemsToRead(itemsToRead),
        m_ExiftoolProcess(NULL),
        m_SettingsModel(settingsModel)
    {
    }

    MetadataReadingWorker::~MetadataReadingWorker() {
        qDebug() << "Reading worker destroyed";

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
                out << line << endl;
            }

            out.flush();
            file.close();

            QString exiftoolPath = m_SettingsModel->getExifToolPath();
            QString exiftoolCommand = QString("%1 -@ %2").arg(exiftoolPath).arg(file.fileName());
            qDebug() << "Starting exiftool with command:" << exiftoolCommand;
            m_ExiftoolProcess->start(exiftoolCommand);

            bool success = m_ExiftoolProcess->waitForFinished();

            if (success && m_ExiftoolProcess->exitStatus() == QProcess::NormalExit) {
                QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
                parseExiftoolOutput(stdoutByteArray);
            }

            if (m_SettingsModel->getSaveBackups()) {
                readBackups();
            }
        }

        emit finished(true);
    }

    void MetadataReadingWorker::cancel() {
        qInfo() << "Cancelling reading metadata...";

        if (m_ExiftoolProcess && m_ExiftoolProcess->state() != QProcess::NotRunning) {
            m_ExiftoolProcess->kill();
            qDebug() << "Exiftool process killed";
        }
    }

    void MetadataReadingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus);
        qDebug() << "Exiftool finished with exitcode" << exitCode;
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

    void MetadataReadingWorker::parseExiftoolOutput(const QByteArray &output) {
        qDebug() << "Parsing JSON output of exiftool...";
        QJsonDocument document = QJsonDocument::fromJson(output);
        if (document.isArray()) {
            QJsonArray filesArray = document.array();

            int size = filesArray.size();
            for (int i = 0; i < size; ++i) {
                const QJsonValue &fileJson = filesArray.at(i);
                if (fileJson.isObject()) {
                    QJsonObject fileObject = fileJson.toObject();
                    ImportDataResult result;
                    jsonObjectToImportResult(fileObject, result);

                    Q_ASSERT(!result.FilePath.isEmpty());
                    Q_ASSERT(!m_ImportResult.contains(result.FilePath));

                    m_ImportResult.insert(result.FilePath, result);
                }
            }
        } else {
            qWarning() << "Exiftool Json Error: Main element is not array";
        }
    }

    void MetadataReadingWorker::readBackups() {
        qDebug() << "Reading backups of items...";
        int size = m_ItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToRead.at(i);
            MetadataSavingCopy copy(metadata);
            if (copy.readFromFile()) {
                const QString &filepath = metadata->getFilepath();
                Q_ASSERT(m_ImportResult.contains(filepath));
                m_ImportResult[filepath].BackupDict = copy.getInfo();
            }
        }
    }
}

