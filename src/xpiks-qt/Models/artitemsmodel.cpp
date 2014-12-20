#include <QStringList>
#include <QDirIterator>
#include <QDebug>
#include <QList>
#include "artitemsmodel.h"
#include "artiteminfo.h"
#include "../Helpers/indiceshelper.h"

namespace Models {

    ArtItemsModel::~ArtItemsModel() {
        qDeleteAll(m_ArtworkList);
        m_ArtworkList.clear();

        // being freed in gui
        //delete m_ArtworksDirectories;
    }

    int ArtItemsModel::getModifiedArtworksCount()
    {
        int modifiedCount = 0;
        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->isModified()) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void ArtItemsModel::updateAllProperties()
    {
        QList<QPair<int, int> > ranges;
        ranges << qMakePair(0, m_ArtworkList.length() - 1);
        QVector<int> roles;
        roles << ArtworkDescriptionRole << KeywordsRole << IsModifiedRole << ArtworkAuthorRole << ArtworkTitleRole;
        updateItemsAtIndices(ranges, roles);
    }

    void ArtItemsModel::removeArtworksDirectory(int index)
    {
        const QString &directory = m_ArtworksRepository->getDirectory(index);
        QList<int> indicesToRemove;

        QList<ArtworkMetadata*>::const_iterator it = m_ArtworkList.constBegin();
        for (int i = 0; it < m_ArtworkList.constEnd(); ++it, ++i) {
            if ((*it)->isInDirectory(directory)) {
                indicesToRemove.append(i);
            }
        }

        doRemoveItemsAtIndices(indicesToRemove);
    }

    void ArtItemsModel::removeKeywordAt(int metadataIndex, int keywordIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeKeywordAt(keywordIndex)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << KeywordsRole << IsModifiedRole);
            }
        }
    }

    void ArtItemsModel::removeLastKeyword(int metadataIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeLastKeyword()) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << KeywordsRole << IsModifiedRole);
            }
        }
    }

    void ArtItemsModel::appendKeyword(int metadataIndex, const QString &keyword)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->appendKeyword(keyword)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << KeywordsRole << IsModifiedRole);
            }
        }
    }

    void ArtItemsModel::setSelectedItemsSaved()
    {
        QList<int> selectedIndices;
        getSelectedItemsIndices(selectedIndices);

        foreach (int index, selectedIndices) {
            m_ArtworkList[index]->unsetModified();
        }

        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        updateItemsAtIndices(rangesToUpdate, QVector<int>() << IsModifiedRole);
    }

    int ArtItemsModel::getSelectedItemsCount()
    {
        QList<int> selectedIndices;
        getSelectedItemsIndices(selectedIndices);
        return selectedIndices.length();
    }

    void ArtItemsModel::removeSelectedArtworks()
    {
        QList<int> indicesToRemove;
        getSelectedItemsIndices(indicesToRemove);
        doRemoveItemsAtIndices(indicesToRemove);
    }

    void ArtItemsModel::updateSelectedArtworks()
    {
        QList<int> selectedIndices;
        getSelectedItemsIndices(selectedIndices);
        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        QVector<int> roles;
        roles << ArtworkDescriptionRole << KeywordsRole << IsModifiedRole << ArtworkAuthorRole << ArtworkTitleRole;
        updateItemsAtIndices(rangesToUpdate, roles);
    }

    void ArtItemsModel::patchSelectedArtworks()
    {
        QList<ArtworkMetadata*> selectedArtworks;
        int count = m_ArtworkList.length();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            if (metadata->getIsSelected()) {
                selectedArtworks.append(metadata);
            }
        }

        // TODO: assert iptc provider is not null
        // TODO: remove this two times copying
        m_IptcProvider->setArtworks(selectedArtworks);
    }

    int ArtItemsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworkList.count();
    }

    QVariant ArtItemsModel::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_ArtworkList.count())
            return QVariant();

        ArtworkMetadata *metadata = m_ArtworkList.at(index.row());
        switch (role) {
        case ArtworkDescriptionRole:
            return metadata->getDescription();
        case ArtworkFilenameRole:
            return metadata->getArtworkFilepath();
        case ArtworkAuthorRole:
            return metadata->getAuthor();
        case ArtworkTitleRole:
            return metadata->getTitle();
        case KeywordsRole:
            return metadata->getKeywords();
        case KeywordsStringRole:
            return metadata->getKeywordsString();
        case IsModifiedRole:
            return metadata->isModified();
        case IsSelectedRole:
            return metadata->getIsSelected();
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags ArtItemsModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool ArtItemsModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid()) {
            return false;
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(index.row());
        switch (role) {
        case EditArtworkDescriptionRole:
            metadata->setDescription(value.toString());
            break;
        case EditArtworkTitleRole:
            metadata->setTitle(value.toString());
            break;
        case EditArtworkAuthorRole:
            metadata->setAuthor(value.toString());
            break;
        case IsSelectedRole:
            metadata->setIsSelected(value.toBool());
            break;
        default:
            return false;
        }

        emit dataChanged(index, index, QVector<int>() << IsModifiedRole << role);
        return true;
    }

    void ArtItemsModel::addLocalArtworks(const QList<QUrl> &artworksPaths)
    {
        qDebug() << artworksPaths;
        QStringList fileList;
        foreach (const QUrl &url, artworksPaths) {
            fileList.append(url.toLocalFile());
        }

        addFiles(fileList);
    }

    void ArtItemsModel::addLocalDirectory(const QUrl &directory)
    {
        qDebug() << directory;
        addDirectory(directory.toLocalFile());
    }

    void ArtItemsModel::addDirectory(const QString &directory)
    {
        QDir dir(directory);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

        QStringList items = dir.entryList();
        for (int i = 0; i < items.size(); ++i) {
            items[i] = dir.filePath(items[i]);
        }

        if (items.count() > 0) {
            addFiles(items);
        }
    }

    void ArtItemsModel::addFiles(const QStringList &rawFilenames)
    {
        QStringList filenames = rawFilenames.filter(QRegExp("^.*[.]jpg$", Qt::CaseInsensitive));

        const int count = filenames.count();
        const int newFilesCount = m_ArtworksRepository->getNewFilesCount(filenames);

        bool filesWereAccounted = m_ArtworksRepository->beginAccountingFiles(filenames);

        if (newFilesCount > 0) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + newFilesCount - 1);

            for (int i = 0; i < count; ++i) {
                const QString &filename = filenames[i];
                if (m_ArtworksRepository->accountFile(filename))
                {
                    ArtworkMetadata *metadata = new ArtworkMetadata(filename);
                    QObject::connect(metadata, SIGNAL(modifiedChanged(bool)),
                                     this, SLOT(itemModifiedChanged(bool)));
                    m_ArtworkList.append(metadata);
                }
            }

            endInsertRows();
        }

        m_ArtworksRepository->endAccountingFiles(filesWereAccounted);

        if (newFilesCount > 0) {
            m_ArtworksRepository->updateCountsForExistingDirectories();
        }

        // set artworks for initial import
        m_IptcProvider->setArtworks(m_ArtworkList);
    }

    void ArtItemsModel::setAllItemsSelected(bool selected)
    {
        int length = m_ArtworkList.length();
        for (int i = 0; i < length; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            metadata->setIsSelected(selected);
        }

        if (length > 0) {
            QModelIndex startIndex = index(0);
            QModelIndex endIndex = index(length - 1);
            emit dataChanged(startIndex, endIndex, QVector<int>() << IsSelectedRole);
        }
    }

    void ArtItemsModel::doCombineSelectedImages(CombinedArtworksModel *combinedModel) const
    {
        QList<ArtItemInfo*> artworksList;

        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->getIsSelected()) {
                ArtItemInfo *info = new ArtItemInfo(metadata);
                artworksList.append(info);
            }
        }

        combinedModel->initArtworks(artworksList);
        combinedModel->recombineArtworks();
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ArtworkDescriptionRole] = "description";
        roles[EditArtworkDescriptionRole] = "editdescription";
        roles[ArtworkAuthorRole] = "author";
        roles[EditArtworkAuthorRole] = "editauthor";
        roles[ArtworkTitleRole] = "title";
        roles[EditArtworkTitleRole] = "edittitle";
        roles[ArtworkFilenameRole] = "filename";
        roles[KeywordsRole] = "keywords";
        roles[KeywordsStringRole] = "keywordsstring";
        roles[IsModifiedRole] = "ismodified";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    void ArtItemsModel::removeInnerItem(int row)
    {
        // TODO: add assert for row
        ArtworkMetadata *metadata = m_ArtworkList[row];
        m_ArtworksRepository->removeFile(metadata->getArtworkFilepath());

        delete metadata;
        m_ArtworkList.removeAt(row);
    }

    void ArtItemsModel::doRemoveItemsAtIndices(const QList<int> &indicesToRemove)
    {
        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);

        m_ArtworksRepository->cleanupEmptyDirectories();
        m_ArtworksRepository->updateCountsForExistingDirectories();
    }

    void ArtItemsModel::getSelectedItemsIndices(QList<int> &indices)
    {
        int count = m_ArtworkList.length();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            if (metadata->getIsSelected()) {
                indices.append(i);
            }
        }
    }
}
