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

namespace Common {
    class BasicMetadataModel;
}

namespace Models {
    class ArtworkMetadata;
}

namespace MetadataIO {
    class MetadataSavingCopy {
    public:
        MetadataSavingCopy() {}
        MetadataSavingCopy(Common::BasicMetadataModel *keywordsModel);
        MetadataSavingCopy(const QHash<QString, QString> &dict);

    public:
        const QHash<QString, QString> &getInfo() const { return m_MetadataInfo; }

        void saveToFile(const QString &filepath) const;
        bool readFromFile(const QString &filepath);
        void saveToMetadata(Models::ArtworkMetadata *artworkMetadata) const;

    private:
        void readFromMetadata(Common::BasicMetadataModel *keywordsModel);

    private:
        QHash<QString, QString> m_MetadataInfo;
    };

    class SaverWorkerJobItem {
    public:
        SaverWorkerJobItem(Models::ArtworkMetadata *metadata);
        virtual ~SaverWorkerJobItem();

    public:
        Models::ArtworkMetadata *getMetadata() const { return m_ArtworkMetadata; }

    private:
        Models::ArtworkMetadata *m_ArtworkMetadata;
    };
}

#endif // SAVERWORKERJOBITEM

