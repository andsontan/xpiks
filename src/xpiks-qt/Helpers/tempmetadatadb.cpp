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

#include "tempmetadatadb.h"

#include <QHash>
#include <QString>
#include <QFile>
#include <QDataStream>
#include "../Models/artworkmetadata.h"
#include "constants.h"

namespace Helpers {
    TempMetadataCopy::TempMetadataCopy(Models::ArtworkMetadata *metadata) {
        m_MetadataInfo["title"] = metadata->getTitle();
        m_MetadataInfo["description"] = metadata->getDescription();
        m_MetadataInfo["keywords"] = metadata->getKeywordsString();
        m_Filepath = metadata->getFilepath();
    }

    void TempMetadataDb::flush() const
    {
        QHash<QString, QString> dict;
        dict["title"] = m_ArtworkMetadata->getTitle();
        dict["description"] = m_ArtworkMetadata->getDescription();
        dict["keywords"] = m_ArtworkMetadata->getKeywordsString();

        QString path = m_ArtworkMetadata->getFilepath() + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data
            out << dict;
            file.close();
        }
    }

    void TempMetadataDb::load() const
    {
        QString path = m_ArtworkMetadata->getFilepath() + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QHash<QString, QString> dict;

            QDataStream in(&file);   // read the data
            in >> dict;
            file.close();

            if (m_ArtworkMetadata->initialize(
                    dict.value("title", ""),
                    dict.value("description", ""),
                    dict.value("keywords", ""),
                    false)) {
                m_ArtworkMetadata->setModified();
            }
        }
    }
}
