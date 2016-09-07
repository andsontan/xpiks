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

#ifndef METADATAELEMENT_H
#define METADATAELEMENT_H

#include <QString>
#include <QSet>
#include "artworkmetadata.h"

namespace Models {
    // kind of smart pointer over ArtworkMetadata
    class MetadataElement
    {
    public:
        MetadataElement(ArtworkMetadata *metadata, int index):
            m_ArtworkMetadata(metadata),
            m_OriginalIndex(index),
            m_IsSelected(false)
        {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->acquire();
            }
        }

        MetadataElement(MetadataElement &&other):
            m_ArtworkMetadata(other.m_ArtworkMetadata),
            m_OriginalIndex(other.m_OriginalIndex),
            m_IsSelected(other.m_IsSelected)
        {
            other.m_ArtworkMetadata = nullptr;
        }

        MetadataElement &operator=(MetadataElement &&other) {
            if (this != &other) {
                releaseArtwork();

                m_ArtworkMetadata = other.m_ArtworkMetadata;
                m_OriginalIndex = other.m_OriginalIndex;
                m_IsSelected = other.m_IsSelected;

                other.m_ArtworkMetadata = nullptr;
            }

            return *this;
        }

        virtual ~MetadataElement() {
            releaseArtwork();
        }

    public:        
        bool isSelected() const { return m_IsSelected; }
        void setSelected(bool selected) { m_IsSelected = selected; }

    public:
        ArtworkMetadata *getOrigin() const { return m_ArtworkMetadata; }
        int getOriginalIndex() const { return m_OriginalIndex; }

#ifdef CORE_TESTS
        void freeMetadata() {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->release();
                m_ArtworkMetadata->deleteLater();
                m_ArtworkMetadata = nullptr;
            }
        }
#endif

    private:
        void releaseArtwork() {
            if (m_ArtworkMetadata != nullptr) {
                m_ArtworkMetadata->release();
            }
        }

        MetadataElement &operator=(const MetadataElement &);
        MetadataElement(const MetadataElement &);

    private:
        ArtworkMetadata *m_ArtworkMetadata;
        int m_OriginalIndex;
        bool m_IsSelected;
    };
}

#endif // METADATAELEMENT_H
