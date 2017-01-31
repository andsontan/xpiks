/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
#include <quazip/JlCompress.h>
#include "filenameshelpers.h"
#include "../Common/defines.h"

namespace Helpers {
    QStringList zipFiles(QStringList filepathes) {
        QString zipFilePath;
        zipArtworkAndVector(filepathes, zipFilePath);
        return filepathes;
    }

    bool zipArtworkAndVector(const QStringList &filepathes, QString &zipFilePath) {
        QString anyFile = filepathes.first();
        QString archivePath = getArchivePath(anyFile);

        bool result = false;
        try {
            result = JlCompress::compressFiles(archivePath, filepathes);
        } catch (...) {
            LOG_WARNING << "Exception while zipping with QuaZip";
        }

        if (!result) {
            LOG_WARNING << "Failed to create zip" << archivePath;
        }

        zipFilePath = archivePath;
        return result;
    }
}


