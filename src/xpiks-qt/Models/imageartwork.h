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

#ifndef IMAGEARTWORK_H
#define IMAGEARTWORK_H

#include "artworkmetadata.h"
#include <QSize>
#include <QString>
#include <QDateTime>
#include "../Common/flags.h"

namespace Models {
    class ImageArtwork: public ArtworkMetadata
    {
        Q_OBJECT
    public:
        ImageArtwork(const QString &filepath, qint64 ID, qint64 directoryID);

    private:
        enum ImageArtworkFlags {
            FlagHasVectorAttached = 1 << 0
        };

        inline bool getHasVectorAttachedFlag() const { return Common::HasFlag(m_ImageFlags, FlagHasVectorAttached); }
        inline void setHasVectorAttachedFlag(bool value) { Common::ApplyFlag(m_ImageFlags, value, FlagHasVectorAttached); }

    public:
        QSize getImageSize() const { return m_ImageSize; }
        void setImageSize(const QSize &size) { m_ImageSize = size; }
        void setDateTimeOriginal(const QDateTime &dateTime) { m_DateTimeOriginal = dateTime; }
        const QString &getAttachedVectorPath() const { return m_AttachedVector; }
        QString getDateTaken() const { return m_DateTimeOriginal.toString(); }
        bool hasVectorAttached() const { return getHasVectorAttachedFlag(); }

    public:
        void attachVector(const QString &vectorFilepath);
        void detachVector();

    private:
        QSize m_ImageSize;
        QString m_AttachedVector;
        QDateTime m_DateTimeOriginal;
        volatile int m_ImageFlags;
    };
}

#endif // IMAGEARTWORK_H
