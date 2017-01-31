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

#include "artworkmetadatabackup.h"
#include "../Models/artworkmetadata.h"
#include "../Models/imageartwork.h"
#include "../Common/defines.h"

UndoRedo::ArtworkMetadataBackup::ArtworkMetadataBackup(Models::ArtworkMetadata *metadata) {
    m_Description = metadata->getDescription();
    m_Title = metadata->getTitle();
    m_KeywordsList = metadata->getKeywords();
    m_IsModified = metadata->isModified();

    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
    if (image != NULL && image->hasVectorAttached()) {
        m_AttachedVector = image->getAttachedVectorPath();
    }
}

UndoRedo::ArtworkMetadataBackup::ArtworkMetadataBackup(const UndoRedo::ArtworkMetadataBackup &copy):
    m_Description(copy.m_Description),
    m_Title(copy.m_Title),
    m_AttachedVector(copy.m_AttachedVector),
    m_KeywordsList(copy.m_KeywordsList),
    m_IsModified(copy.m_IsModified)
{
}

void UndoRedo::ArtworkMetadataBackup::restore(Models::ArtworkMetadata *metadata) const {
    metadata->setDescription(m_Description);
    metadata->setTitle(m_Title);
    metadata->setKeywords(m_KeywordsList);
    if (m_IsModified) { metadata->setModified(); }
    else { metadata->resetModified(); }

    if (!m_AttachedVector.isEmpty()) {
        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
        if (image != NULL) {
            image->attachVector(m_AttachedVector);
        } else {
            LOG_WARNING << "Inconsistency for attached vector";
        }
    }
}
