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
#include "../Commands/addartworkscommand.h"
#include "../Commands/removeartworkscommand.h"

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
        const QString &directory = m_CommandManager->getArtworksRepository()->getDirectory(index);
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

        // TODO: remove this two times copying
        m_CommandManager->setArtworksForIPTCProcessing(modifiedSelectedArtworks);
    }

    void ArtItemsModel::setSelectedForUpload()
    {
        QList<ArtworkMetadata*> selectedArtworks;
        getSelectedArtworks(selectedArtworks);

        // TODO: remove this two times copying
        m_CommandManager->setArtworksForUpload(selectedArtworks);
        emit needCheckItemsForWarnings(selectedArtworks);
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
        const ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();
        const QString directory = artworksRepository->getDirectory(directoryIndex);

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
            emit needCheckItemsForWarnings(m_ArtworkList);
            qDebug() << "Checking all items for upload warnings...";
        } else {
            QList<ArtworkMetadata*> selectedArtworks;
            getSelectedArtworks(selectedArtworks);
            qDebug() << "Selected items: " << selectedArtworks.length();
            emit needCheckItemsForWarnings(selectedArtworks);
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

        if (role == EditArtworkDescriptionRole ||
                role == EditArtworkTitleRole ||
                role == EditArtworkAuthorRole) {
            metadata->saveBackup();
        }

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
        qDebug() << "Adding local directory: " << directory;
        int addedFilesCount = addDirectory(directory.toLocalFile());
        return addedFilesCount;
    }

    void ArtItemsModel::itemSelectedChanged(bool value)
    {
        int plus = value ? +1 : -1;
        m_SelectedArtworksCount += plus;
        emit selectedArtworksCountChanged();
    }

    void ArtItemsModel::beginAccountingFiles(int filesCount)
    {
        int rowsCount = rowCount();
        beginInsertRows(QModelIndex(), rowsCount, rowsCount + filesCount - 1);
    }

    void ArtItemsModel::beginAccountingFiles(int start, int end)
    {
        beginInsertRows(QModelIndex(), start, end);
    }

    void ArtItemsModel::endAccountingFiles()
    {
        endInsertRows();
    }

    void ArtItemsModel::insertArtwork(int index, ArtworkMetadata *metadata)
    {
        Q_ASSERT(index >= 0 && index <= m_ArtworkList.length());
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.insert(index, metadata);
    }

    void ArtItemsModel::appendArtwork(ArtworkMetadata *metadata)
    {
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.append(metadata);
    }

    ArtworkMetadata *ArtItemsModel::getArtwork(int index) const
    {
        ArtworkMetadata *result = NULL;
        if (index >= 0 && index < m_ArtworkList.length()) {
            result = m_ArtworkList[index];
        }

        return result;
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

        Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames);
        Commands::CommandResult *result = m_CommandManager->processCommand(addArtworksCommand);
        Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
        int newFilesCount = addArtworksResult->m_NewFilesAdded;
        delete result;

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

    void ArtItemsModel::doCombineSelectedImages() const
    {
        QList<ArtItemInfo*> artworksList;

        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            if (metadata->getIsSelected()) {
                ArtItemInfo *info = new ArtItemInfo(metadata);
                artworksList.append(info);
            }
        }

        m_CommandManager->combineArtworks(artworksList);
    }

    void ArtItemsModel::doCombineArtwork(int index)
    {
        if (index >= 0 && index < m_ArtworkList.length()) {
            QList<ArtItemInfo*> artworksList;

            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            metadata->setIsSelected(true);
            QModelIndex qmIndex = this->index(index);
            emit dataChanged(qmIndex, qmIndex, QVector<int>() << IsSelectedRole);

            ArtItemInfo *info = new ArtItemInfo(metadata);
            artworksList.append(info);

            m_CommandManager->combineArtworks(artworksList);
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
        Q_ASSERT(row >= 0 && row < m_ArtworkList.length());
        ArtworkMetadata *metadata = m_ArtworkList[row];
        ArtworksRepository *artworkRepository = m_CommandManager->getArtworksRepository();
        artworkRepository->removeFile(metadata->getFilepath());

        if (metadata->getIsSelected()) {
            m_SelectedArtworksCount--;
            emit selectedArtworksCountChanged();
        }

        delete metadata;
        m_ArtworkList.removeAt(row);
    }

    void ArtItemsModel::doRemoveItemsAtIndices(const QList<int> &indicesToRemove)
    {
        Commands::RemoveArtworksCommand *removeArtworksCommand = new Commands::RemoveArtworksCommand(indicesToRemove);
        Commands::CommandResult *result = m_CommandManager->processCommand(removeArtworksCommand);
        delete result;
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
