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

#include "saverworkerjobitem.h"
#include <QFile>
#include <QDataStream>
#include "../Helpers/constants.h"

namespace MetadataIO {
    MetadataSavingCopy::MetadataSavingCopy(Models::ArtworkMetadata *metadata):
        m_Filepath(metadata->getFilepath()),
        m_Metadata(metadata)
    {
    }

    MetadataSavingCopy::MetadataSavingCopy(Models::ArtworkMetadata *metadata, const QHash<QString, QString> &dict):
        m_Filepath(metadata->getFilepath()),
        m_Metadata(metadata),
        m_MetadataInfo(dict)
    {
    }

    void MetadataSavingCopy::readFromMetadata() {
        m_MetadataInfo["title"] = m_Metadata->getTitle();
        m_MetadataInfo["description"] = m_Metadata->getDescription();
        m_MetadataInfo["keywords"] = m_Metadata->getKeywordsString();
    }

    void MetadataSavingCopy::saveToFile() const {
        QString path = m_Filepath + QLatin1String(Constants::METADATA_BACKUP_EXTENSION);
        const QHash<QString, QString> &dict = m_MetadataInfo;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data
            out << dict;
            file.close();
        }
    }

    bool MetadataSavingCopy::readFromFile() {
        bool success = false;
        QString path = m_Filepath + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QHash<QString, QString> dict;

            QDataStream in(&file);   // read the data
            in >> dict;
            file.close();

            success = true;

            m_MetadataInfo.swap(dict);
        }

        return success;
    }

    void MetadataSavingCopy::saveToMetadata() const {
        const QHash<QString, QString> &dict = m_MetadataInfo;

        QString keywordsString = dict.value("keywords", "");
        QStringList keywords = keywordsString.split(QChar(','), QString::SkipEmptyParts);

        if (m_Metadata->initialize(
                dict.value("title", ""),
                dict.value("description", ""),
                keywords,
                false)) {
            m_Metadata->markModified();
        }
    }
}

