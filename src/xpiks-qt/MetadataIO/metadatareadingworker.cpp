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

#include "metadatareadingworker.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QImageReader>
#include "../Models/settingsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/constants.h"
#include "saverworkerjobitem.h"
#include "../Common/defines.h"

#ifdef Q_OS_WIN
#define _X86_
#include <fileapi.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#define SOURCEFILE QLatin1String("SourceFile")
#define TITLE QLatin1String("Title")
#define DESCRIPTION QLatin1String("Description")
#define OBJECTNAME QLatin1String("ObjectName")
#define IMAGEDESCRIPTION QLatin1String("ImageDescription")
#define CAPTIONABSTRACT QLatin1String("Caption-Abstract")
#define KEYWORDS QLatin1String("Keywords")
#define SUBJECT QLatin1String("Subject")
#define DATETAKEN QLatin1String("DateTimeOriginal")
#define TIMEZONE QLatin1String("TimeZoneOffset")

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

    bool parseStringKeywords(QString &keywords, ImportDataResult &result) {
        bool parsed = false;
        if (!keywords.isEmpty()) {
            // old Xpiks bug when it called exiftool with wrong arguments
            if (keywords.startsWith(QChar('"')) && keywords.endsWith('"')) {
                keywords = keywords.mid(1, keywords.length() - 2);
            }

            result.Keywords = keywords.split(QChar(','), QString::SkipEmptyParts);
            parsed = true;
        }

        return parsed;
    }

    int parseTimeZoneOffset(const QJsonObject &object) {
        // NOT the standard tag
        // from the spec it contains 1 or 2 values
        QJsonValue timeZone = object.value(TIMEZONE);
        int timeZoneOffset;
        if (timeZone.isArray()) {
            QJsonArray timeZoneArray = timeZone.toArray();
            timeZoneOffset = timeZoneArray.at(0).toInt();
        } else {
            timeZoneOffset = timeZone.toInt();
        }

        return timeZoneOffset;
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

        if (object.contains(DATETAKEN)) {
            QString dateTime = object.value(DATETAKEN).toString();
            result.DateTimeOriginal = QDateTime::fromString(dateTime, Qt::ISODate);
        }

        bool keywordsSet = false;

        if (object.contains(KEYWORDS)) {
            QJsonValue keywords = object.value(KEYWORDS);
            if (keywords.isArray()) {
                parseJsonKeywords(keywords.toArray(), result);
                keywordsSet = true;
            } else {
                LOG_WARNING << "Keywords object in json is not array";
                QString keywordsStr = object.value(KEYWORDS).toString().trimmed();
                keywordsSet = parseStringKeywords(keywordsStr, result);
            }
        }

        if (!keywordsSet && object.contains(SUBJECT)) {
            QJsonValue subject = object.value(SUBJECT);
            if (subject.isArray()) {
                parseJsonKeywords(subject.toArray(), result);
            } else {
                QString keywordsStr = object.value(SUBJECT).toString().trimmed();
                parseStringKeywords(keywordsStr, result);
            }
        }
    }

    MetadataReadingWorker::MetadataReadingWorker(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                                 Models::SettingsModel *settingsModel,
                                                 const QVector<QPair<int, int> > &rangesToUpdate):
        m_ItemsToRead(itemsToRead),
        m_ExiftoolProcess(NULL),
        m_RangesToUpdate(rangesToUpdate),
        m_SettingsModel(settingsModel)
    {
    }

    MetadataReadingWorker::~MetadataReadingWorker() {
        LOG_DEBUG << "Reading worker destroyed";
    }

    void MetadataReadingWorker::process() {
        bool success = false;
        initWorker();

        QTemporaryFile argumentsFile;

        if (argumentsFile.open()) {
            LOG_INFO << "Created arguments file" << argumentsFile.fileName();

            QStringList exiftoolArguments = createArgumentsList();
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
#else
            int fsyncResult = fsync(argumentsFile.handle());
            LOG_DEBUG << "fsync result:" << fsyncResult;
#endif
            argumentsFile.close();

            QString exiftoolPath = m_SettingsModel->getExifToolPath();
            QStringList arguments;
#ifdef Q_OS_WIN
            arguments << "-charset" << "FileName=UTF8";

            /*
            QString exiftoolCommand = QString("\"\"%1\" -charset filename=UTF8 -@ \"%2\"\"").arg(exiftoolPath).arg(tempFilename);
            LOG_DEBUG << "Windows command is:" << exiftoolCommand;
            m_ExiftoolProcess->setTextModeEnabled(false);
            m_ExiftoolProcess->setProgram("cmd.exe");
            m_ExiftoolProcess->setArguments(QStringList() << "/U" << "/c");
            m_ExiftoolProcess->setNativeArguments(exiftoolCommand);
            m_ExiftoolProcess->start();
            */
#endif
            arguments << "-@" << argumentsFile.fileName();
            LOG_INFO << "Starting exiftool process:" << exiftoolPath;
            m_ExiftoolProcess->start(exiftoolPath, arguments);

            success = m_ExiftoolProcess->waitForFinished();
            LOG_DEBUG << "Exiftool process finished.";

            int exitCode = m_ExiftoolProcess->exitCode();
            QProcess::ExitStatus exitStatus = m_ExiftoolProcess->exitStatus();

            success = success &&
                    (exitCode == 0) &&
                    (exitStatus == QProcess::NormalExit);

            LOG_INFO << "Exiftool exitcode =" << exitCode << "exitstatus =" << exitStatus;
            LOG_DEBUG << "Temporary file:" << argumentsFile.fileName();

            QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
            parseExiftoolOutput(stdoutByteArray);

            if (m_SettingsModel->getSaveBackups()) {
                readBackupsAndSizes(success);
            } else {
                readSizes();
            }
        }

        emit finished(success);
    }

    void MetadataReadingWorker::cancel() {
        LOG_INFO << "Cancelling...";

        if (m_ExiftoolProcess && m_ExiftoolProcess->state() != QProcess::NotRunning) {
            m_ExiftoolProcess->kill();
            LOG_INFO << "Exiftool process killed";
        }
    }

    void MetadataReadingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus);
        LOG_DEBUG << "Exiftool finished with exitcode" << exitCode;

        QByteArray stderrByteArray = m_ExiftoolProcess->readAllStandardError();
        QString stderrText = QString::fromUtf8(stderrByteArray);
        LOG_DEBUG << "STDERR [Exiftool]:" << stderrText;
    }

    void MetadataReadingWorker::initWorker() {
        m_ExiftoolProcess = new QProcess(this);

        QObject::connect(m_ExiftoolProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(innerProcessFinished(int,QProcess::ExitStatus)));
    }

    QStringList MetadataReadingWorker::createArgumentsList() {
        QStringList arguments;
        arguments.reserve(m_ItemsToRead.length() + 10);

        /*
         * Related to the hack in windows for UTF8-encoded paths
#ifdef Q_OS_WIN
        arguments << "-charset" << "FileName=UTF8";
#endif
*/
        arguments << "-json" << "-ignoreMinorErrors" << "-e";
        arguments << "-ObjectName" << "-Title";
        arguments << "-ImageDescription" << "-Description" << "-Caption-Abstract";
        arguments << "-Keywords" << "-Subject";
        arguments << "-DateTimeOriginal" << "-TimeZoneOffset";
        int size = m_ItemsToRead.length();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToRead.at(i);
            arguments << metadata->getFilepath();
        }

        return arguments;
    }

    void MetadataReadingWorker::parseExiftoolOutput(const QByteArray &output) {
        LOG_DEBUG << "Parsing JSON output of exiftool...";
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
                    LOG_DEBUG << "Parsed file:" << result.FilePath;
                }
            }
        } else {
            LOG_WARNING << "Exiftool Output Parsing Error";
        }
    }

    void MetadataReadingWorker::readBackupsAndSizes(bool exiftoolSuccess) {
        LOG_DEBUG << "exiftool success is" << exiftoolSuccess;
        int size = m_ItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            MetadataSavingCopy copy;
            if (copy.readFromFile(filepath)) {
                if (exiftoolSuccess) {
                    Q_ASSERT(m_ImportResult.contains(filepath));
                }

                m_ImportResult[filepath].BackupDict = copy.getInfo();
            }

            ImportDataResult &importResultItem = m_ImportResult[filepath];

            QImageReader reader(filepath);
            importResultItem.ImageSize = reader.size();

            QFileInfo fi(filepath);
            importResultItem.FileSize = fi.size();
        }
    }

    void MetadataReadingWorker::readSizes() {
        LOG_DEBUG << "#";
        int size = m_ItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = m_ItemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            Q_ASSERT(m_ImportResult.contains(filepath));
            ImportDataResult &importResultItem = m_ImportResult[filepath];

            QImageReader reader(filepath);
            importResultItem.ImageSize = reader.size();

            QFileInfo fi(filepath);
            importResultItem.FileSize = fi.size();
        }
    }
}

