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

#include "saverworkerjobitem.h"
#include <QFile>
#include <QDataStream>
#include "../Helpers/constants.h"
#include "../Common/basickeywordsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"

namespace MetadataIO {
    MetadataSavingCopy::MetadataSavingCopy(Common::BasicKeywordsModel *keywordsModel) {
        readFromMetadata(keywordsModel);
    }

    MetadataSavingCopy::MetadataSavingCopy(const QHash<QString, QString> &dict):
        m_MetadataInfo(dict)
    {
    }

    void MetadataSavingCopy::saveToFile(const QString &filepath) const {
        QString path = filepath + QLatin1String(Constants::METADATA_BACKUP_EXTENSION);
        const QHash<QString, QString> &dict = m_MetadataInfo;
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data
            out << dict;
            file.close();
        }
    }

    bool MetadataSavingCopy::readFromFile(const QString &filepath) {
        bool success = false;
        QString path = filepath + Constants::METADATA_BACKUP_EXTENSION;
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QHash<QString, QString> dict;

            QDataStream in(&file);   // read the data
            try {
                in >> dict;
                m_MetadataInfo.swap(dict);
                success = true;
            }
            catch (...) {
                LOG_WARNING << "Exception while reading backup";
            }

            file.close();
        }

        return success;
    }

    void MetadataSavingCopy::saveToMetadata(Models::ArtworkMetadata *artworkMetadata) const {
        const QHash<QString, QString> &dict = m_MetadataInfo;

        QString keywordsString = dict.value("keywords", "");
        QStringList keywords = keywordsString.split(QChar(','), QString::SkipEmptyParts);

        if (artworkMetadata->initialize(
                dict.value("title", ""),
                dict.value("description", ""),
                keywords,
                false)) {
            artworkMetadata->markModified();
        }
    }

    void MetadataSavingCopy::readFromMetadata(Common::BasicKeywordsModel *keywordsModel) {
        m_MetadataInfo["title"] = keywordsModel->getTitle();
        m_MetadataInfo["description"] = keywordsModel->getDescription();
        m_MetadataInfo["keywords"] = keywordsModel->getKeywordsString();
    }

    SaverWorkerJobItem::SaverWorkerJobItem(Models::ArtworkMetadata *metadata):
        m_ArtworkMetadata(metadata)
    {
        if (m_ArtworkMetadata != nullptr) {
            m_ArtworkMetadata->acquire();
        }
    }

    SaverWorkerJobItem::~SaverWorkerJobItem() {
        if (m_ArtworkMetadata != nullptr) {
            m_ArtworkMetadata->release();
        }
    }
}

