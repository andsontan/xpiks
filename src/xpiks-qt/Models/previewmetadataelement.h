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
            m_ShowKeywords(false)
        {}

        PreviewMetadataElement(PreviewMetadataElement &&other):
            MetadataElement(std::move(other)),
            m_ShowTitle(false),
            m_ShowDescription(false),
            m_ShowKeywords(false) {}

        PreviewMetadataElement &operator=(PreviewMetadataElement &&other) {
            m_ShowTitle = other.m_ShowTitle;
            m_ShowDescription = other.m_ShowDescription;
            m_ShowKeywords = other.m_ShowKeywords;

            return static_cast<PreviewMetadataElement &>( MetadataElement::operator=( std::move(other) ) );
        }

        virtual ~PreviewMetadataElement() {}

    public:
        bool isDescriptionShowable() const { return m_ShowDescription; }
        bool isTitleShowable() const { return m_ShowTitle; }
        bool isKeywordsShowable() const { return m_ShowKeywords; }
        void setShowTitle(bool show) { m_ShowTitle = show; }
        void setShowDescription(bool show) { m_ShowDescription = show; }
        void setShowKeywords(bool show) { m_ShowKeywords = show; }

    private:
        bool m_ShowTitle;
        bool m_ShowDescription;
        bool m_ShowKeywords;
    };
}

#endif // PREVIEWMETADATAELEMENT_H
