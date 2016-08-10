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

#include "filenameshelpers.h"
#include <QRegExp>
#include <QFileInfo>
#include <QDir>

QStringList Helpers::convertToVectorFilenames(const QStringList &item) {
    QStringList converted;
    converted.reserve(item.length()*2);
    QRegExp regExp("(.*)[.](jpg|jpeg|tiff)", Qt::CaseInsensitive);

    foreach (const QString &item, item) {
        QString replacedEPS = QString(item).replace(regExp, "\\1.eps");
        QString replacedAI = QString(item).replace(regExp, "\\1.ai");

        if (replacedEPS != item) {
            converted << replacedEPS;
        }

        if (replacedAI != item) {
            converted << replacedAI;
        }
    }

    return converted;
}

QString Helpers::getArchivePath(const QString &artworkPath) {
    QFileInfo fi(artworkPath);
    QString archiveName = fi.baseName() + ".zip";

    QString basePath = fi.absolutePath();
    QDir dir(basePath);
    QString archivePath = dir.filePath(archiveName);
    return archivePath;
}

QString Helpers::getImagePath(const QString &path) {
    QString result = path;

    QRegExp regExp("(.*)[.](ai|eps|zip)", Qt::CaseInsensitive);
    result.replace(regExp, "\\1.jpg");

    return result;
}
