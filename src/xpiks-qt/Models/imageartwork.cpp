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

#include "imageartwork.h"
#include "../Common/defines.h"

namespace Models {
    ImageArtwork::ImageArtwork(const QString &filepath, qint64 ID, qint64 directoryID):
        ArtworkMetadata(filepath, ID, directoryID),
        m_ImageFlags(0)
    {
    }

    void ImageArtwork::attachVector(const QString &vectorFilepath) {
        LOG_INFO << "Attaching vector file:" << vectorFilepath << "to file" << getFilepath();
        setHasVectorAttachedFlag(true);
        m_AttachedVector = vectorFilepath;
    }

    void ImageArtwork::detachVector() {
        LOG_INFO << "#";
        setHasVectorAttachedFlag(false);
        m_AttachedVector.clear();
    }
}
