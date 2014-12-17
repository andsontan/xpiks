#ifndef ARTITEMINFO_H
#define ARTITEMINFO_H

#include <QString>

namespace Models {
    class ArtItemInfo
    {
    public:
        ArtItemInfo(int index, const QString &filepath) :
            m_Filepath(filepath), m_DependentIndex(index), m_IsSelected(false) {}
        ~ArtItemInfo() {}

    private:
        QString m_Filepath;
        // index in main viewmodel of artworks
        int m_DependentIndex;
        bool m_IsSelected;
    };
}

#endif // ARTITEMINFO_H
