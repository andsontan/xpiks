#include "combinedartworksmodel.h"
#include "../Helpers/indiceshelper.h"

namespace Models {
    CombinedArtworksModel::CombinedArtworksModel(QObject *parent)
    {
    }

    void CombinedArtworksModel::recombineArtworks()
    {
        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        QString description;
        QSet<QString> commonKeywords;

        int artworksCount = m_ArtworksList.length();
        for (int i = 0; i < artworksCount; ++i) {
            ArtItemInfo *info = m_ArtworksList[i];

            if (!anyItemsProcessed) {
                description = info->getDescription();
                commonKeywords.unite(info->getKeywords());
                anyItemsProcessed = true;
                continue;
            }

            const QString &currDescription = info->getDescription();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            commonKeywords.intersect(info->getKeywords());
        }

        if (artworksCount > 0) {
            if (descriptionsDiffer) {
                description = "";
            }

            initDescription(description);
            initKeywords(commonKeywords.toList());
        }
    }

    void CombinedArtworksModel::resetModelData()
    {
        this->beginResetModel();
        qDeleteAll(m_ArtworksList);
        m_ArtworksList.clear();
        this->endResetModel();

        setDescription("");
        setKeywords(QStringList());
    }

    void CombinedArtworksModel::removeKeywordAt(int keywordIndex)
    {
        if (keywordIndex >= 0 && keywordIndex < m_CommonKeywords.length()) {
            m_CommonKeywords.removeAt(keywordIndex);
            emit keywordsChanged();
        }
    }

    void CombinedArtworksModel::appendKeyword(const QString &keyword)
    {
        m_CommonKeywords.append(keyword);
        emit keywordsChanged();
    }

    void CombinedArtworksModel::selectArtwork(int index)
    {
        if (index < 0 || index >= m_ArtworksList.length()) {
            return;
        }

        m_ArtworksList[index]->select();
        QModelIndex qIndex = this->index(index);
        emit dataChanged(qIndex, qIndex, QVector<int>() << IsSelectedRole);
    }

    void CombinedArtworksModel::removeSelectedArtworks()
    {
        int count = m_ArtworksList.length();
        QList<int> indicesToRemove;
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                indicesToRemove.append(i);
            }
        }

        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);

        recombineArtworks();
    }

    int CombinedArtworksModel::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_ArtworksList.count();
    }

    QVariant CombinedArtworksModel::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_ArtworksList.count())
            return QVariant();

        ArtItemInfo *artItemInfo = m_ArtworksList.at(index.row());

        switch (role) {
        case PathRole:
            return artItemInfo->getFilePath();
        case IsSelectedRole:
            return artItemInfo->isSelected();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> CombinedArtworksModel::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void CombinedArtworksModel::removeInnerItem(int row)
    {
        ArtItemInfo *info = m_ArtworksList[row];
        delete info;
        m_ArtworksList.removeAt(row);
    }
}
