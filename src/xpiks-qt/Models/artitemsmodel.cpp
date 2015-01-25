/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#include <QStringList>
#include <QDirIterator>
#include <QImageReader>
#include <QDebug>
#include <QList>
#include "artitemsmodel.h"
#include "artiteminfo.h"
#include "../Helpers/indiceshelper.h"

#ifdef Q_OS_OSX
#include "../Helpers/osxnsurlhelper.h"
#endif

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
        roles << ArtworkDescriptionRole << IsModifiedRole << ArtworkAuthorRole << ArtworkTitleRole << KeywordsCountRole;
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
        emit selectedArtworksCountChanged();
        emit modifiedArtworksCountChanged();
    }

    void ArtItemsModel::removeKeywordAt(int metadataIndex, int keywordIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeKeywordAt(keywordIndex)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
            }
        }
    }

    void ArtItemsModel::removeLastKeyword(int metadataIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeLastKeyword()) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
            }
        }
    }

    void ArtItemsModel::appendKeyword(int metadataIndex, const QString &keyword)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->appendKeyword(keyword)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
            }
        }
    }

    void ArtItemsModel::backupItem(int metadataIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            metadata->saveBackup();
        }
    }

    int ArtItemsModel::dropFiles(const QList<QUrl> &urls)
    {
#ifdef Q_OS_MAC
        QList<QUrl> localUrls;
        foreach (const QUrl &url, urls) {
            QUrl localUrl = Helpers::fromNSUrl(url);
            localUrls.append(localUrl);
        }

        int count = addLocalArtworks(localUrls);
        return count;
#else
        int count = addLocalArtworks(urls);
        return count;
#endif
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

        updateModifiedCount();
        emit artworksChanged();
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
        roles << ArtworkDescriptionRole << IsModifiedRole <<
                 ArtworkAuthorRole << ArtworkTitleRole << KeywordsCountRole;
        updateItemsAtIndices(rangesToUpdate, roles);

        emit artworksChanged();
    }

    void ArtItemsModel::patchSelectedArtworks()
    {
        QList<ArtworkMetadata*> modifiedSelectedArtworks;
        int count = m_ArtworkList.length();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList[i];
            if (metadata->getIsSelected() &&
                    metadata->isModified()) {
                modifiedSelectedArtworks.append(metadata);
            }
        }

        // TODO: assert iptc provider is not null
        // TODO: remove this two times copying
        m_IptcProvider->setArtworks(modifiedSelectedArtworks);
    }

    void ArtItemsModel::uploadSelectedArtworks()
    {
        QList<ArtworkMetadata*> selectedArtworks;
        getSelectedArtworks(selectedArtworks);

        // TODO: assert uploader is not null
        // TODO: remove this two times copying
        m_ArtworkUploader->setArtworks(selectedArtworks);
        m_WarningsManager->checkForWarnings(selectedArtworks);
    }

    bool ArtItemsModel::areSelectedArtworksSaved()
    {
        bool areModified = false;

        foreach(ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->getIsSelected()) {
                if (metadata->isModified()) {
                    areModified = true;
                    break;
                }
            }
        }

        return !areModified;
    }

    void ArtItemsModel::selectDirectory(int directoryIndex)
    {
        QList<int> directoryItems;
        const QString directory = m_ArtworksRepository->getDirectory(directoryIndex);

        int i = 0;
        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->isInDirectory(directory)) {
                directoryItems.append(i);
                metadata->setIsSelected(!metadata->getIsSelected());
            }

            i++;
        }

        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(directoryItems, rangesToUpdate);
        updateItemsAtIndices(rangesToUpdate, QVector<int>() << IsSelectedRole);
    }

    void ArtItemsModel::checkForWarnings()
    {
        if (this->getSelectedArtworksCount() == 0) {
            m_WarningsManager->checkForWarnings(m_ArtworkList);
            qDebug() << "Checking all items for upload warnings...";
        } else {
            QList<ArtworkMetadata*> selectedArtworks;
            getSelectedArtworks(selectedArtworks);
            qDebug() << "Selected items: " << selectedArtworks.length();
            m_WarningsManager->checkForWarnings(selectedArtworks);
            qDebug() << "Checking selected items for upload warnings...";
        }
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
            return metadata->getFilepath();
        case ArtworkAuthorRole:
            return metadata->getAuthor();
        case ArtworkTitleRole:
            return metadata->getTitle();
        case KeywordsStringRole:
            return metadata->getKeywordsString();
        case IsModifiedRole:
            return metadata->isModified();
        case IsSelectedRole:
            return metadata->getIsSelected();
        case KeywordsCountRole:
            return metadata->getKeywordsCount();
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
        int roleToUpdate = 0;
        switch (role) {
        case EditArtworkDescriptionRole:
            metadata->setDescription(value.toString());
            roleToUpdate = ArtworkDescriptionRole;
            break;
        case EditArtworkTitleRole:
            metadata->setTitle(value.toString());
            roleToUpdate = ArtworkTitleRole;
            break;
        case EditArtworkAuthorRole:
            metadata->setAuthor(value.toString());
            roleToUpdate = ArtworkAuthorRole;
            break;
        case EditIsSelectedRole:
            metadata->setIsSelected(value.toBool());
            roleToUpdate = IsSelectedRole;
            break;
        default:
            return false;
        }

        emit dataChanged(index, index, QVector<int>() << IsModifiedRole << role << roleToUpdate);
        metadata->saveBackup();
        return true;
    }

    int ArtItemsModel::addLocalArtworks(const QList<QUrl> &artworksPaths)
    {
        qDebug() << artworksPaths;
        QStringList fileList;
        foreach (const QUrl &url, artworksPaths) {
            fileList.append(url.toLocalFile());
        }

        int filesAddedCount = addFiles(fileList);
        return filesAddedCount;
    }

    int ArtItemsModel::addLocalDirectory(const QUrl &directory)
    {
        qDebug() << directory;
        int addedFilesCount = addDirectory(directory.toLocalFile());
        return addedFilesCount;
    }

    void ArtItemsModel::itemSelectedChanged(bool value)
    {
        int plus = value ? +1 : -1;
        m_SelectedArtworksCount += plus;
        emit selectedArtworksCountChanged();
    }

    int ArtItemsModel::addDirectory(const QString &directory)
    {
        int filesCount = 0;

        QDir dir(directory);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

        QStringList items = dir.entryList();
        for (int i = 0; i < items.size(); ++i) {
            items[i] = dir.filePath(items[i]);
        }

        if (items.count() > 0) {
            filesCount = addFiles(items);
        }

        return filesCount;
    }

    int ArtItemsModel::addFiles(const QStringList &rawFilenames)
    {
        QStringList filenames;

        foreach (const QString &filepath, rawFilenames) {
            QImageReader imageReader(filepath);
            if (imageReader.format() == "jpeg") {
                filenames.append(filepath);
            }
        }

        const int count = filenames.count();
        const int newFilesCount = m_ArtworksRepository->getNewFilesCount(filenames);

        bool filesWereAccounted = m_ArtworksRepository->beginAccountingFiles(filenames);

        QList<ArtworkMetadata*> artworksToImport;

        if (newFilesCount > 0) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + newFilesCount - 1);

            for (int i = 0; i < count; ++i) {
                const QString &filename = filenames[i];
                if (m_ArtworksRepository->accountFile(filename))
                {
                    ArtworkMetadata *metadata = new ArtworkMetadata(filename);
                    QObject::connect(metadata, SIGNAL(modifiedChanged(bool)),
                                     this, SLOT(itemModifiedChanged(bool)));
                    QObject::connect(metadata, SIGNAL(selectedChanged(bool)),
                                     this, SLOT(itemSelectedChanged(bool)));                    
                    QObject::connect(metadata, SIGNAL(fileSelectedChanged(QString,bool)),
                                     m_ArtworksRepository, SLOT(fileSelectedChanged(QString,bool)));

                    m_ArtworkList.append(metadata);
                    artworksToImport.append(metadata);
                }
            }

            endInsertRows();
        }

        m_ArtworksRepository->endAccountingFiles(filesWereAccounted);

        if (newFilesCount > 0) {
            m_ArtworksRepository->updateCountsForExistingDirectories();
        }

        // set artworks for initial import
        m_IptcProvider->setArtworks(artworksToImport);

        return newFilesCount;
    }

    void ArtItemsModel::setAllItemsSelected(bool selected)
    {
        qDebug() << "Setting all items selected (" << selected << ")";
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

    void ArtItemsModel::getSelectedArtworks(QList<ArtworkMetadata *> &selectedArtworks) const
    {
        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->getIsSelected()) {
                selectedArtworks.append(metadata);
            }
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

    void ArtItemsModel::doCombineArtwork(int index, CombinedArtworksModel *combinedModel)
    {
        if (index >= 0 && index < m_ArtworkList.length()) {
            QList<ArtItemInfo*> artworksList;

            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            metadata->setIsSelected(true);
            QModelIndex qmIndex = this->index(index);
            emit dataChanged(qmIndex, qmIndex, QVector<int>() << IsSelectedRole);

            ArtItemInfo *info = new ArtItemInfo(metadata);
            artworksList.append(info);

            combinedModel->initArtworks(artworksList);
            combinedModel->recombineArtworks();
        }
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
        roles[KeywordsStringRole] = "keywordsstring";
        roles[IsModifiedRole] = "ismodified";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[KeywordsCountRole] = "keywordscount";
        return roles;
    }

    void ArtItemsModel::removeInnerItem(int row)
    {
        // TODO: add assert for row
        ArtworkMetadata *metadata = m_ArtworkList[row];
        m_ArtworksRepository->removeFile(metadata->getFilepath());

        if (metadata->getIsSelected()) {
            m_SelectedArtworksCount--;
            emit selectedArtworksCountChanged();
        }

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
        updateModifiedCount();
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
