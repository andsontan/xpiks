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
#include <QDebug>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"

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
        QString title = metadata->getTitle().trimmed();
        QString description = metadata->getDescription().trimmed();

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
        qDebug() << "Writing worker destroyed";
    }

    void MetadataWritingWorker::process() {
        bool success = false;

        initWorker();

        QTemporaryFile jsonFile;
        if (jsonFile.open()) {
            qDebug() << "Serializing artworks to json";
            QJsonArray objectsToSave;
            artworksToJsonArray(m_ItemsToWrite, objectsToSave);
            QJsonDocument document(objectsToSave);
            jsonFile.write(document.toJson());
            jsonFile.flush();
            jsonFile.close();

            QTemporaryFile argumentsFile;
            if (argumentsFile.open()) {
                QTextStream out(&argumentsFile);
                QStringList exiftoolArguments = createArgumentsList(jsonFile.fileName());
                foreach (const QString &line, exiftoolArguments) {
                    out << line << endl;
                }

                out.flush();
                argumentsFile.close();

                QString exiftoolPath = m_SettingsModel->getExifToolPath();
                QStringList arguments;
                arguments << "-IPTC:CodedCharacterSet=UTF8" << "-@" << argumentsFile.fileName();
                argumentsFile.close();

                m_ExiftoolProcess->start(exiftoolPath, arguments);
                success = m_ExiftoolProcess->waitForFinished();
                success = success &&
                        (m_ExiftoolProcess->exitCode() == 0) &&
                        (m_ExiftoolProcess->exitStatus() == QProcess::NormalExit);
            }
        }

        emit finished(success);
    }

    void MetadataWritingWorker::innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus);
        qDebug() << "Exiftool finished with exitcode" << exitCode;

        QByteArray stdoutByteArray = m_ExiftoolProcess->readAllStandardOutput();
        QString stdoutText(stdoutByteArray);
        qDebug() << "STDOUT [ExifTool]:" << stdoutText;

        QByteArray stderrByteArray = m_ExiftoolProcess->readAllStandardError();
        QString stderrText(stderrByteArray);
        qDebug() << "STDERR [Exiftool]:" << stderrText;
    }

    void MetadataWritingWorker::initWorker() {
        m_ExiftoolProcess = new QProcess(this);

        QObject::connect(m_ExiftoolProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(innerProcessFinished(int,QProcess::ExitStatus)));
    }

    QStringList MetadataWritingWorker::createArgumentsList(const QString &jsonFilePath) {
        QStringList arguments;
        arguments.reserve(m_ItemsToWrite.length() + 5);

        arguments << "-j=" + jsonFilePath;

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
