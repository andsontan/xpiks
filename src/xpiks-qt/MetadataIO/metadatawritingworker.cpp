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

#include "metadatawritingworker.h"
#include "../Common/defines.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QTextStream>
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"
#include "../Common/defines.h"

#ifdef Q_OS_WIN
#define _X86_
#include <fileapi.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define SOURCEFILE QLatin1String("SourceFile")
#define XMP_TITLE QLatin1String("XMP:Title")
#define IPTC_OBJECTNAME QLatin1String("IPTC:ObjectName")
#define XMP_DESCRIPTION QLatin1String("XMP:Description")
#define EXIF_IMAGEDESCRIPTION QLatin1String("EXIF:ImageDescription")
#define IPTC_CAPTIONABSTRACT QLatin1String("IPTC:Caption-Abstract")
#define IPTC_KEYWORDS QLatin1String("IPTC:Keywords")
#define XMP_SUBJECT QLatin1String("XMP:Subject")

namespace MetadataIO {
    void keywordsToJsonArray(const QStringList &keywords, QJsonArray &array) {
        int size = keywords.size();
        for (int i = 0; i < size; ++i) {
            array.append(QJsonValue(keywords.at(i)));
        }
    }

    void metadataToJsonObject(Models::ArtworkMetadata *metadata, QJsonObject &jsonObject) {
        QString title = metadata->getTitle().simplified();
        QString description = metadata->getDescription().simplified();

        if (title.isEmpty()) {
            title = description;
        }

        jsonObject.insert(SOURCEFILE, QJsonValue(metadata->getFilepath()));

        QJsonValue titleValue(title);
        jsonObject.insert(XMP_TITLE, titleValue);
        jsonObject.insert(IPTC_OBJECTNAME, titleValue);

        QJsonValue descriptionValue(description);
        jsonObject.insert(XMP_DESCRIPTION, descriptionValue);
        jsonObject.insert(EXIF_IMAGEDESCRIPTION, descriptionValue);
        jsonObject.insert(IPTC_CAPTIONABSTRACT, descriptionValue);

        const QStringList &keywords = metadata->getKeywords();
        QJsonArray keywordsArray;
        keywordsToJsonArray(keywords, keywordsArray);
        jsonObject.insert(IPTC_KEYWORDS, keywordsArray);
        jsonObject.insert(XMP_SUBJECT, keywordsArray);
    }

    void artworksToJsonArray(const QVector<Models::ArtworkMetadata *> &itemsToWrite, QJsonArray &array) {
        int size = itemsToWrite.size();
        for (int i = 0; i < size; ++i) {
            QJsonObject artworkObject;
            metadataToJsonObject(itemsToWrite.at(i), artworkObject);
            array.append(artworkObject);
        }
    }

    MetadataWritingWorker::MetadataWritingWorker(const QVector<Models::ArtworkMetadata *> &itemsToWrite,
                                                 Models::SettingsModel *settingsModel, bool useBackups):
        m_ItemsToWrite(itemsToWrite),
        m_ExiftoolProcess(NULL),
        m_SettingsModel(settingsModel),
        m_UseBackups(useBackups)
    {
    }

    MetadataWritingWorker::~MetadataWritingWorker() {
        LOG_DEBUG << "destroyed";
    }

    void MetadataWritingWorker::process() {
        bool success = false;

        initWorker();

        QTemporaryFile jsonFile;
        if (jsonFile.open()) {
            LOG_INFO << "Serializing artworks to json" << jsonFile.fileName();
            QJsonArray objectsToSave;
            artworksToJsonArray(m_ItemsToWrite, objectsToSave);
            QJsonDocument document(objectsToSave);
            jsonFile.write(document.toJson());
            jsonFile.flush();
            jsonFile.close();

            int numberOfItems = m_ItemsToWrite.length();

            QTemporaryFile argumentsFile;
            if (argumentsFile.open()) {
                QStringList exiftoolArguments = createArgumentsList(jsonFile.fileName());

                foreach (const QString &line, exiftoolArguments) {
                    argumentsFile.write(line.toUtf8());
#ifdef Q_OS_WIN
                    argumentsFile.write("\r\n");
#else
                    argumentsFile.write("\n");
#endif
                }
                argumentsFile.flush();

                LOG_DEBUG << "Waiting for tempfile bytes written...";
    #ifdef Q_OS_WIN
                HANDLE fileHandle = (HANDLE)_get_osfhandle(argumentsFile.handle());
                bool flushResult = FlushFileBuffers(fileHandle);
                LOG_DEBUG << "Windows flush result:" << flushResult;
                // TODO: FIXME: Windows bug with UTF-8 and Exiftool
                QThread::sleep(1);
    #else
                int fsyncResult = fsync(argumentsFile.handle());
                LOG_DEBUG << "fsync result:" << fsyncResult;
    #endif
                argumentsFile.close();

                QString exiftoolPath = m_SettingsModel->getExifToolPath();
                QStringList arguments;
                arguments << "-charset" << "FileName=UTF8";
                arguments << "-IPTC:CodedCharacterSet=UTF8" << "-@" << argumentsFile.fileName();

                LOG_DEBUG << "Starting exiftool process:" << exiftoolPath;

                m_ExiftoolProcess->start(exiftoolPath, arguments);
                const int oneFileTimeout = 5000;
                success = m_ExiftoolProcess->waitForFinished(oneFileTimeout*numberOfItems);

                LOG_DEBUG << "Exiftool process finished.";

                int exitCode = m_ExiftoolProcess->exitCode();
                QProcess::ExitStatus exitStatus = m_ExiftoolProcess->exitStatus();

                success = success &&
                        (exitCode == 0) &&
                        (exitStatus == QProcess::NormalExit);

                LOG_INFO << "Exiftool exitcode =" << exitCode << "exitstatus =" << exitStatus;
                LOG_DEBUG << "Temporary file:" << argumentsFile.fileName();

                if (!success) {
                    LOG_WARNING << "Exiftool error string:" << m_ExiftoolProcess->errorString();
                }
            }
        }

        emit finished(success);
    }

    void MetadataWritingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus);
        LOG_DEBUG << "Exiftool finished with exitcode" << exitCode;

        QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
        QString stdoutText(stdoutByteArray);
        LOG_DEBUG << "STDOUT [ExifTool]:" << stdoutText;

        QByteArray stderrByteArray = m_ExiftoolProcess->readAllStandardError();
        QString stderrText(stderrByteArray);
        LOG_DEBUG << "STDERR [Exiftool]:" << stderrText;
    }

    void MetadataWritingWorker::initWorker() {
        m_ExiftoolProcess = new QProcess(this);

        QObject::connect(m_ExiftoolProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(innerProcessFinished(int,QProcess::ExitStatus)));
    }

    QStringList MetadataWritingWorker::createArgumentsList(const QString &jsonFilePath) {
        QStringList arguments;
        arguments.reserve(m_ItemsToWrite.length() + 5);

        // ignore minor warnings
        arguments << "-m" << "-j=" + jsonFilePath;

        if (!m_UseBackups) {
            arguments << "-overwrite_original";
        }

        int size = m_ItemsToWrite.length();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToWrite.at(i);
            arguments << metadata->getFilepath();
        }

        return arguments;
    }
}
