#ifndef ARTITEMINFO_H
#define ARTITEMINFO_H

#include <QString>
#include <QSet>
#include "artworkmetadata.h"

namespace Models {
    class ArtItemInfo
    {
    public:
        ArtItemInfo(ArtworkMetadata *metadata) :
            m_ArtworkMetadata(metadata),
            m_IsSelected(false)
        {}
        ~ArtItemInfo() {}

    public:
        const QSet<QString> &getKeywords() const { return m_ArtworkMetadata->getKeywordsSet(); }
        const QString &getFilePath() const { return m_ArtworkMetadata->getFilepath(); }
        const QString &getDescription() const { return m_ArtworkMetadata->getDescription(); }
        const QString &getTitle() const { return m_ArtworkMetadata->getTitle(); }
        bool isSelected() const { return m_IsSelected; }
        void select() { m_IsSelected = true; }
        void deselect() { m_IsSelected = false; }

    public:
        void addKeywordsToOrigin(const QStringList &keywordsList) { m_ArtworkMetadata->appendKeywords(keywordsList); }
        void setKeywordsToOrigin(const QStringList &keyowrdsList) { m_ArtworkMetadata->setKeywords(keyowrdsList); }
        void setDescriptionToOrigin(const QString &description) { m_ArtworkMetadata->setDescription(description); }
        void setTitleToOrigin(const QString &title) { m_ArtworkMetadata->setTitle(title); }

    private:
        ArtworkMetadata *m_ArtworkMetadata;
        bool m_IsSelected;
    };
}

#endif // ARTITEMINFO_H
