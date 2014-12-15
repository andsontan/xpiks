#include "combinedartworksmodel.h"

namespace Models {
    CombinedArtworksModel::CombinedArtworksModel(QObject *parent)
    {

    }

    void CombinedArtworksModel::setDescription(const QString &description)
    {
        m_ImageDescription = description;
    }

    void CombinedArtworksModel::removeKeywordAt(int keywordIndex)
    {
        if (keywordIndex >= 0 && keywordIndex < m_CommonKeywords.length()) {
            m_CommonKeywords.removeAt(keywordIndex);
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &keyword)
    {
        m_CommonKeywords.append(keyword);
    }

    int CombinedArtworksModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_ImagesFilenames.count();
    }

    QVariant CombinedArtworksModel::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_ImagesFilenames.count())
            return QVariant();

        const QString &filename = m_ImagesFilenames.at(index.row());

        switch (role) {
        case PathRole:
            return filename;
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> CombinedArtworksModel::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        return roles;
    }
}
