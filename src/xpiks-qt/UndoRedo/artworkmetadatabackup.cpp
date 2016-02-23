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

#include "artworkmetadatabackup.h"
#include "../Models/artworkmetadata.h"

UndoRedo::ArtworkMetadataBackup::ArtworkMetadataBackup(Models::ArtworkMetadata *metadata)
{
    m_Description = metadata->getDescription();
    m_Title = metadata->getTitle();
    m_KeywordsList = metadata->getKeywords();
    m_IsModified = metadata->isModified();
    if (metadata->hasVectorAttached()) {
        m_AttachedVector = metadata->getAttachedVectorPath();
    }
}

void UndoRedo::ArtworkMetadataBackup::restore(Models::ArtworkMetadata *metadata) const
{
    metadata->setDescription(m_Description);
    metadata->setTitle(m_Title);
    metadata->setKeywords(m_KeywordsList);
    if (m_IsModified) { metadata->setModified(); }
    else { metadata->resetModified(); }

    if (!m_AttachedVector.isEmpty()) {
        metadata->attachVector(m_AttachedVector);
    }
}
