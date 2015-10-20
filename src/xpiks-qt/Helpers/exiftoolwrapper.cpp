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

#include "exiftoolwrapper.h"

#include <QByteArray>
#include <QTextCodec>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include <QDebug>
#include <QPair>
#include "../Models/artworkmetadata.h"
#include "../Models/exportinfo.h"
#include "tempmetadatadb.h"
#include "Helpers/settingsprovider.h"
#include "../Models/settingsmodel.h"

// returns NULL if patching wasn't successfull
ExportPair writeArtworkMetadata(ExportPair pair) {
    Models::ArtworkMetadata *metadata = pair.first;
    Models::ExportInfo *exportInfo = pair.second;

    QString title = QString(metadata->getTitle()).simplified();
    QString description = QString(metadata->getDescription()).simplified();
    const QString &keywords = metadata->getKeywordsString();

    if (title.simplified().isEmpty()) {
        title = description;
    }

    Models::SettingsModel *settingsModel = Helpers::SettingsProvider::getInstance().getSettingsModelInstance();
    const QString exiftoolPath = settingsModel->getExifToolPath();

    QStringList arguments;
    arguments << exiftoolPath;
    arguments << "-quiet" << "-ignoreMinorErrors";
    arguments << "-charset exif=UTF8" << "-charset iptc=UTF8";
    arguments << QString("-IPTC:ObjectName=\"%1\"").arg(title);
    arguments << QString("-XMP:Title=\"%1\"").arg(title);
    arguments << QString("-EXIF:ImageDescription=\"%1\"").arg(description);
    arguments << QString("-IPTC:LocalCaption=\"%1\"").arg(description);
    arguments << QString("-IPTC:Caption-Abstract=\"%1\"").arg(description);
    arguments << QString("-XMP:Caption=\"%1\"").arg(description);
    arguments << QString("-XMP:Description=\"%1\"").arg(description);
    arguments << QString("-IPTC:Keywords=\"%1\"").arg(keywords);
    arguments << QString("-XMP:Keywords=\"%1\"").arg(keywords);
    arguments << QString("-XMP:Subject=\"%1\"").arg(keywords);
    arguments << QString("-IPTC:CodedCharacterSet=\"UTF8\"");

    if (!exportInfo->getMustSaveOriginal()) {
        arguments << "-overwrite_original";
    }

    arguments << QString("\"%1\"").arg(metadata->getFilepath());

    Models::ArtworkMetadata *resultMetadata = NULL;

    QProcess process;
    QString command = arguments.join(' ');

    process.start(command);
    bool finished = process.waitForFinished();

    if (!finished || process.exitStatus() != QProcess::NormalExit) {
        QByteArray stdoutByteArray = process.readAll();
        QString stdoutText(stdoutByteArray);
        if (!stdoutText.isEmpty()) {
            qDebug() << "Error:" << exiftoolPath << stdoutText;
        }

        qDebug() << "Error:" << exiftoolPath << process.errorString();
        return qMakePair(resultMetadata, exportInfo);
    } else {
        resultMetadata = metadata;
        return qMakePair(resultMetadata, exportInfo);
    }
}

void grabMetadata(const QStringList &items, Models::ImportDataResult *importData,
                  QRegExp titleRegExp,
                  QRegExp descriptionRegExp,
                  QRegExp keywordsRegExp);

ImportPair readArtworkMetadata(ImportPair pair) {
    Models::SettingsModel *settingsModel = Helpers::SettingsProvider::getInstance().getSettingsModelInstance();
    const QString exiftoolPath = settingsModel->getExifToolPath();

    Models::ImportDataResult *importData = pair.second;
    Models::ArtworkMetadata *metadata = pair.first;

    QStringList arguments;
    arguments << "-s" << "-e" << "-n" << "-EXIF:all" << "-IPTC:all" << "-XMP:all";
    arguments << "-charset exif=UTF8" << "-charset iptc=UTF8";
    arguments << metadata->getFilepath();

    QProcess process;
    process.start(exiftoolPath, arguments);
    bool finished = process.waitForFinished();

    QByteArray stdoutByteArray = process.readAll();
    QString stdoutText = QString::fromUtf8(stdoutByteArray.data());

    if (!finished || process.exitStatus() != QProcess::NormalExit) {
        if (!stdoutText.isEmpty()) {
            qDebug() << "Error:" << exiftoolPath << stdoutText;
        }

        qDebug() << "Error:" << exiftoolPath << process.errorString();
        return ImportPair(metadata, NULL);
    }

    QStringList items = stdoutText.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

    QRegExp titleRegExp("^ObjectName\\s|^Title\\s");
    QRegExp descriptionRegExp("^ImageDescription\\s|^Caption-Abstract\\s|^Description\\s");
    QRegExp keywordsRegExp("^Keywords\\s|^Subject\\s");

    grabMetadata(items, importData, titleRegExp, descriptionRegExp, keywordsRegExp);

    return pair;
}

void grabMetadata(const QStringList &items, Models::ImportDataResult *importData,
                  QRegExp titleRegExp,
                  QRegExp descriptionRegExp,
                  QRegExp keywordsRegExp) {
    bool titleSet = false, descriptionSet = false, keywordsSet = false;

    foreach (const QString &item, items) {
        QStringList parts = item.split(':', QString::SkipEmptyParts);
        if (parts.empty()) {
            continue;
        }

        const QString &first = parts.first();

        if (!titleSet && first.contains(titleRegExp)) {
            importData->Title = parts.at(1).simplified();
            titleSet = true;
        }
        else if (!descriptionSet && first.contains(descriptionRegExp)) {
            importData->Description = parts.at(1).simplified();
            descriptionSet = true;
        }
        else if (!keywordsSet && first.contains(keywordsRegExp)) {
            importData->Keywords = parts.at(1).simplified();
            keywordsSet = true;
        }
    }
}
