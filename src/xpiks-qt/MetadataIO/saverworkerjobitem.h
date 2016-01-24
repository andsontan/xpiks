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

#ifndef SAVERWORKERJOBITEM
#define SAVERWORKERJOBITEM

#include <QHash>
#include <QString>
#include "../Models/artworkmetadata.h"

namespace MetadataIO {
    class MetadataSavingCopy {
    public:
        MetadataSavingCopy(Models::ArtworkMetadata *metadata);
        MetadataSavingCopy(Models::ArtworkMetadata *metadata, const QHash<QString, QString> &dict);

    public:
        const QString &getFilepath() const { return m_Filepath; }
        const QHash<QString, QString> &getInfo() const { return m_MetadataInfo; }

        void readFromMetadata();
        void saveToFile() const;
        bool readFromFile();
        void saveToMetadata() const;

    private:
        QHash<QString, QString> m_MetadataInfo;
        QString m_Filepath;
        Models::ArtworkMetadata *m_Metadata;
    };

    enum SaverWorkerJobType { JobTypeRead, JobTypeWrite };

    class SaverWorkerJobItem {
    public:
        SaverWorkerJobItem(Models::ArtworkMetadata *metadata, SaverWorkerJobType jobType):
            m_ArtworkMetadata(metadata),
            m_JobType(jobType)
        {
        }

    public:
        SaverWorkerJobType getJobType() const { return m_JobType; }
        Models::ArtworkMetadata *getMetadata() const { return m_ArtworkMetadata; }

    private:
        Models::ArtworkMetadata *m_ArtworkMetadata;
        SaverWorkerJobType m_JobType;
    };
}

#endif // SAVERWORKERJOBITEM

