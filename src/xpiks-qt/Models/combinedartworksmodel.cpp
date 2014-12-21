#include "combinedartworksmodel.h"
#include "../Helpers/indiceshelper.h"

namespace Models {
    void CombinedArtworksModel::initArtworks(const QList<ArtItemInfo *> &artworks)
    {
        int innerLength = m_ArtworksList.length();
        int start = innerLength == 0 ? 0 : innerLength - 1;
        int paramLength = artworks.length();
        if (paramLength > 0) {
            beginInsertRows(QModelIndex(), start, start + paramLength - 1);
            m_ArtworksList.append(artworks);
            endInsertRows();
        }
    }

    void CombinedArtworksModel::recombineArtworks()
    {
        bool anyItemsProcessed = false;
        bool descriptionsDiffer = false;
        bool titleDiffer = false;
        QString description;
        QString title;
        QSet<QString> commonKeywords;

        int artworksCount = m_ArtworksList.length();
        for (int i = 0; i < artworksCount; ++i) {
            ArtItemInfo *info = m_ArtworksList[i];

            if (!anyItemsProcessed) {
                description = info->getDescription();
                title = info->getTitle();
                commonKeywords.unite(info->getKeywords());
                anyItemsProcessed = true;
                continue;
            }

            const QString &currDescription = info->getDescription();
            const QString &currTitle = info->getTitle();
            descriptionsDiffer = descriptionsDiffer || description != currDescription;
            titleDiffer = titleDiffer || title != currTitle;
            commonKeywords.intersect(info->getKeywords());
        }

        if (artworksCount > 0) {
            if (descriptionsDiffer) {
                description = "";
            }

            if (titleDiffer) {
                title = "";
            }

            initDescription(description);
            initTitle(title);
            initKeywords(commonKeywords.toList());
        }
    }

    void CombinedArtworksModel::resetModelData()
    {
        beginResetModel();
        qDeleteAll(m_ArtworksList);
        m_ArtworksList.clear();
        endResetModel();

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

    void CombinedArtworksModel::deselectArtwork(int index)
    {
        if (index < 0 || index >= m_ArtworksList.length()) {
            return;
        }

        m_ArtworksList[index]->deselect();
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

    int CombinedArtworksModel::getSelectedArtworksCount() const
    {
        int selectedCount = 0;
        int count = m_ArtworksList.length();
        for (int i = 0; i < count; ++i) {
            ArtItemInfo *item = m_ArtworksList[i];
            if (item->isSelected()) {
                selectedCount++;
            }
        }

        return selectedCount;
    }

    void CombinedArtworksModel::saveSetKeywords()
    {
        foreach (ArtItemInfo* info, m_ArtworksList) {
            info->setKeywordsToOrigin(m_CommonKeywords);
            info->setDescriptionToOrigin(m_ArtworkDescription);
            info->setTitleToOrigin(m_ArtworkTitle);
        }
    }

    void CombinedArtworksModel::saveAddKeywords()
    {
        foreach (ArtItemInfo* info, m_ArtworksList) {
            info->addKeywordsToOrigin(m_CommonKeywords);
            info->setDescriptionToOrigin(m_ArtworkDescription);
            info->setTitleToOrigin(m_ArtworkTitle);
        }
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
