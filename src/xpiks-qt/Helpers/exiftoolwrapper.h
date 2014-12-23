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

#ifndef EXIFTOOLWRAPPER
#define EXIFTOOLWRAPPER

#include <QByteArray>
#include <QProcess>
#include <QRegExp>
#include <QString>
#include <QPair>
#include "externaltoolsprovider.h"
#include "../Models/artworkmetadata.h"
#include "../Models/exportinfo.h"

typedef QPair<Models::ArtworkMetadata*, Models::ExportInfo*> ExportPair;

// returns NULL if patching wasn't successfull
ExportPair writeArtworkMetadata(ExportPair pair) {
    Models::ArtworkMetadata *metadata = pair.first;
    Models::ExportInfo *exportInfo = pair.second;

    const QString &author = metadata->getAuthor();
    const QString &title = metadata->getTitle();
    const QString &description = metadata->getDescription();
    const QString &keywords = metadata->getKeywordsString();

    QStringList arguments;
    arguments << "-quiet" << "-ignoreMinorErrors";
    arguments << QString("-EXIF:Artist=%1").arg(author);
    arguments << QString("-IPTC:By-line=%1").arg(author);
    arguments << QString("-XMP:Author=%1").arg(author);
    arguments << QString("-XMP:Creator=%1").arg(author);
    arguments << QString("-IPTC:ObjectName=%1").arg(title);
    arguments << QString("-XMP:Title=%1").arg(title);
    arguments << QString("-EXIF:ImageDescription=%1").arg(description);
    arguments << QString("-IPTC:LocalCaption=%1").arg(description);
    arguments << QString("-IPTC:Caption-Abstract=%1").arg(description);
    arguments << QString("-XMP:Caption=%1").arg(description);
    arguments << QString("-XMP:Description=%1").arg(description);
    arguments << QString("-IPTC:Keywords=%1").arg(keywords);
    arguments << QString("-XMP:Keywords=%1").arg(keywords);
    arguments << QString("-XMP:Subject=%1").arg(keywords);

    if (!exportInfo->getMustSaveOriginal()) {
        arguments << "-overwrite_original";
    }

    arguments << metadata->getFilepath();

    const QString exiftoolPath = Helpers::ExternalToolsProvider::getExifToolPath();

    Models::ArtworkMetadata *resultMetadata = NULL;

    QProcess process;
    process.start(exiftoolPath, arguments);
    if (!process.waitForFinished()) {
        return qMakePair(resultMetadata, exportInfo);
    }

    if (process.exitStatus() == QProcess::NormalExit) {
        resultMetadata = metadata;
    }

    return qMakePair(resultMetadata, exportInfo);
}

void grabMetadata(const QStringList &items, Models::ArtworkMetadata *metadata);

Models::ArtworkMetadata *readArtworkMetadata(Models::ArtworkMetadata *metadata) {
    const QString exiftoolPath = Helpers::ExternalToolsProvider::getExifToolPath();

    QStringList arguments;
    arguments << "-s" << "-e" << "-n" << "-EXIF:all" << "-IPTC:all" << "-XMP:all";
    arguments << metadata->getFilepath();

    QProcess process;
    process.start(exiftoolPath, arguments);
    if (!process.waitForFinished()) {
        return NULL;
    }

    QByteArray stdoutByteArray = process.readAll();
    QString stdoutTextText(stdoutByteArray);
    QStringList items = stdoutTextText.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    grabMetadata(items, metadata);

    return metadata;
}

void grabMetadata(const QStringList &items, Models::ArtworkMetadata *metadata) {
    bool authorSet = false, titleSet = false, descriptionSet = false, keywordsSet = false;

    QRegExp authorRegExp("^Artist\\s|^By-line\\s|^Creator\\s");
    QRegExp titleRegExp("^ObjectName\\s|^Title\\s");
    QRegExp descriptionRegExp("^ImageDescription\\s|^Caption-Abstract\\s|^Description\\s");
    QRegExp keywordsRegExp("^Keywords\\s|^Subject\\s");

    QString author, title, description, keywords;

    foreach (const QString &item, items) {
        QStringList parts = item.split(':', QString::SkipEmptyParts);
        if (parts.empty()) {
            continue;
        }

        const QString &first = parts.first();

        if (!authorSet && first.contains(authorRegExp)) {
            author = parts.at(1).trimmed();
            authorSet = true;
        }
        else if (!titleSet && first.contains(titleRegExp)) {
            title = parts.at(1).trimmed();
            titleSet = true;
        }
        else if (!descriptionSet && first.contains(descriptionRegExp)) {
            description = parts.at(1).trimmed();
            descriptionSet = true;
        }
        else if (!keywordsSet && first.contains(keywordsRegExp)) {
            keywords = parts.at(1).trimmed();
            keywordsSet = true;
        }
    }

    metadata->initialize(author, title, description, keywords);
}

#endif // EXIFTOOLWRAPPER

