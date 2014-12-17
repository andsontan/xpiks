#ifndef ARTITEMINFO_H
#define ARTITEMINFO_H

#include <QString>
#include <QSet>
#include "artworkmetadata.h"

namespace Models {
    class ArtItemInfo
    {
    public:
        ArtItemInfo(int index, ArtworkMetadata *metadata) :
            m_ArtworkMetadata(metadata),
            m_DependentIndex(index),
            m_IsSelected(false)
        {}
        ~ArtItemInfo() {}

    public:
        const QSet<QString> &getKeywords() const { return m_ArtworkMetadata->getKeywordsSet(); }
        const QString &getFilePath() const { return m_ArtworkMetadata->getImageFileName(); }
        const QString &getDescription() const { return m_ArtworkMetadata->getImageDescription(); }
        bool isSelected() const { return m_IsSelected; }
        void select() { m_IsSelected = true; }
        void deselect() { m_IsSelected = false; }

    private:
        ArtworkMetadata *m_ArtworkMetadata;
        // index in main viewmodel of artworks
        int m_DependentIndex;
        bool m_IsSelected;
    };
}

#endif // ARTITEMINFO_H
