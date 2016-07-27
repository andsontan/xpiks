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

#ifndef PREVIEWMETADATAELEMENT_H
#define PREVIEWMETADATAELEMENT_H

#include "metadataelement.h"

namespace Models {
    class PreviewMetadataElement:
        public MetadataElement
    {
    public:
        PreviewMetadataElement(ArtworkMetadata *metadata, int index):
            MetadataElement(metadata, index),
            m_ShowTitle(false),
            m_ShowDescription(false),
            m_ShowKeywords(false),
            m_IsReplacable(true)
        {}

        PreviewMetadataElement(PreviewMetadataElement &&other):
            MetadataElement(std::move(other)),
            m_ShowTitle(false),
            m_ShowDescription(false),
            m_ShowKeywords(false),
            m_IsReplacable(true)
        {}

        PreviewMetadataElement &operator=(PreviewMetadataElement &&other) {
            m_ShowTitle = other.m_ShowTitle;
            m_ShowDescription = other.m_ShowDescription;
            m_ShowKeywords = other.m_ShowKeywords;
            m_IsReplacable = other.m_IsReplacable;

            return static_cast<PreviewMetadataElement &>(MetadataElement::operator=(std::move(other)));
        }

        virtual ~PreviewMetadataElement() {}

    public:
        bool isDescriptionShowable() const { return m_ShowDescription; }
        bool isTitleShowable() const { return m_ShowTitle; }
        bool isKeywordsShowable() const { return m_ShowKeywords; }
        bool isReplacable() const { return m_IsReplacable; }
        void setShowTitle(bool show) { m_ShowTitle = show; }
        void setShowDescription(bool show) { m_ShowDescription = show; }
        void setShowKeywords(bool show) { m_ShowKeywords = show; }
        void setIsReplacable(bool replace) { m_IsReplacable = replace; }

    private:
        bool m_ShowTitle;
        bool m_ShowDescription;
        bool m_ShowKeywords;
        bool m_IsReplacable;
    };
}

#endif // PREVIEWMETADATAELEMENT_H
