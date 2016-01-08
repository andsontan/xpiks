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

#include "backupsaverworker.h"
#include <QFile>
#include <QDataStream>
#include "../Helpers/constants.h"
#include "../Helpers/tempmetadatadb.h"

namespace MetadataIO {
    bool BackupSaverWorker::processOneItem(SaverWorkerJobItem *item) {
        Q_ASSERT(item != NULL);

        SaverWorkerJobType jobType = item->getJobType();

        if (jobType == JobTypeRead) {
            readItem(item);
        } else if (jobType == JobTypeWrite) {
            writeItem(item);
        }

        return true;
    }

    void BackupSaverWorker::readItem(SaverWorkerJobItem *item) {
        Models::ArtworkMetadata *metadata = item->getMetadata();

        QString path = metadata->getFilepath() + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QHash<QString, QString> dict;

            QDataStream in(&file);   // read the data
            in >> dict;
            file.close();

            QString keywordsString = dict.value("keywords", "");
            QStringList keywords = keywordsString.split(QChar(','), QString::SkipEmptyParts);

            if (metadata->initialize(
                    dict.value("title", ""),
                    dict.value("description", ""),
                    keywords,
                    false)) {
                metadata->markModified();
            }
        }
    }

    void BackupSaverWorker::writeItem(SaverWorkerJobItem *item) {
        Helpers::TempMetadataCopy copy(item->getMetadata());

        QString path = copy.getFilepath() + QLatin1String(Constants::METADATA_BACKUP_EXTENSION);
        const QHash<QString, QString> &dict = copy.getInfo();
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data
            out << dict;
            file.close();
        }
    }
}
