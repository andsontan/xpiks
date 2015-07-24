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

#include "ziphelper.h"
#include <QFileInfo>
#include <QDebug>
#include "../quazip/quazip/JlCompress.h"

namespace Helpers {
    QStringList zipFiles(QStringList filepathes) {
        QString zipFilePath;
        zipArtworkAndEPS(filepathes, zipFilePath);
        return filepathes;
    }

    bool zipArtworkAndEPS(const QStringList &filepathes, QString &zipFilePath) {
        QString anyFile = filepathes.first();
        QString archivePath = getArchivePath(anyFile);

        bool result = JlCompress::compressFiles(archivePath, filepathes);
        if (!result) {
            qDebug() << "Failed to create zip" << archivePath;
        }

        zipFilePath = archivePath;
        return result;
    }

    QString getArchivePath(const QString &artworkPath) {
        QFileInfo fi(artworkPath);
        QString archiveName = fi.baseName() + ".zip";

        QString basePath = fi.absolutePath();
        QDir dir(basePath);
        QString archivePath = dir.filePath(archiveName);
        return archivePath;
    }
}


