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

#ifndef TEMPMETADATADB_H
#define TEMPMETADATADB_H

#include <QHash>
#include <QString>

namespace Models {
    class ArtworkMetadata;
}

namespace Helpers {
    class TempMetadataCopy {
    public:
        TempMetadataCopy(Models::ArtworkMetadata *metadata);

    public:
        const QString &getFilepath() const { return m_Filepath; }
        const QHash<QString, QString> &getInfo() const { return m_MetadataInfo; }

    private:
        QHash<QString, QString> m_MetadataInfo;
        QString m_Filepath;
    };

    // class not functions because it's supposed to progress in future
    class TempMetadataDb
    {
    public:
        TempMetadataDb(Models::ArtworkMetadata *metadata) :
            m_ArtworkMetadata(metadata)
        {  }

    public:
        // deprecated. use backupsaverworker's queue instead
        void flush() const;
        void load() const;

    private:
        Models::ArtworkMetadata *m_ArtworkMetadata;
    };
}

#endif // TEMPMETADATADB_H
