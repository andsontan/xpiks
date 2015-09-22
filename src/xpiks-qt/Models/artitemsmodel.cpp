/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Commands/pastekeywordscommand.h"
#include "../Suggestion/keywordssuggestor.h"
#include "../Commands/commandmanager.h"
#include "artworksrepository.h"
#include "../Models/settingsmodel.h"

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

    ArtworkMetadata *ArtItemsModel::createMetadata(const QString &filepath) const { return new ArtworkMetadata(filepath); }

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

    void ArtItemsModel::updateItems(const QList<int> &indices, const QVector<int> &roles) {
        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(indices, rangesToUpdate);
        updateItemsInRanges(rangesToUpdate, roles);
    }

    void ArtItemsModel::forceUnselectAllItems() const {
        foreach (ArtworkMetadata *metadata, m_ArtworkList) {
            metadata->resetSelected();
        }
    }

    void ArtItemsModel::updateAllProperties()
    {
        QList<QPair<int, int> > ranges;
        ranges << qMakePair(0, m_ArtworkList.length() - 1);
        QVector<int> roles;
        roles << ArtworkDescriptionRole << IsModifiedRole << ArtworkTitleRole << KeywordsCountRole;
        updateItemsInRanges(ranges, roles);
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

        emit modifiedArtworksCountChanged();
    }

    void ArtItemsModel::removeKeywordAt(int metadataIndex, int keywordIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeKeywordAt(keywordIndex)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                backupItem(metadataIndex);
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

    void ArtItemsModel::pasteKeywords(int metadataIndex, const QStringList &keywords)
    {
        if (metadataIndex >= 0
                && metadataIndex < m_ArtworkList.length()
                && !keywords.empty()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            ArtItemInfo *artItemInfo = new ArtItemInfo(metadata, metadataIndex);

            Commands::PasteKeywordsCommand *pasteCommand = new Commands::PasteKeywordsCommand(artItemInfo, keywords);
            Commands::CommandResult *result = m_CommandManager->processCommand(pasteCommand);
            delete result;

            QModelIndex index = this->index(metadataIndex);
            emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
        }
    }

    void ArtItemsModel::backupItem(int metadataIndex)
    {
        if (metadataIndex >= 0 && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            SettingsModel *settings = m_CommandManager->getSettingsModel();
            metadata->saveBackup(settings);
        }
    }

    int ArtItemsModel::dropFiles(const QList<QUrl> &urls)
    {
        QList<QUrl> localUrls;

#ifdef Q_OS_MAC
        foreach (const QUrl &url, urls) {
            QUrl localUrl = Helpers::fromNSUrl(url);
            localUrls.append(localUrl);
        }
#else
        localUrls = urls;
#endif
        QList<QUrl> directories, files;

        foreach(const QUrl &url, localUrls) {
            bool isDirectory = QDir(url.toLocalFile()).exists();
            if (isDirectory) {
                directories.append(url);
            } else {
                files.append(url);
            }
        }

        int count = 0;
        count += addLocalArtworks(files);
        count += addLocalDirectories(directories);

        return count;
    }

    void ArtItemsModel::setSelectedItemsSaved(const QList<int> &selectedIndices) {
        foreach (int index, selectedIndices) {
            m_ArtworkList[index]->unsetModified();
        }

        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        updateItemsInRanges(rangesToUpdate, QVector<int>() << IsModifiedRole);

        updateModifiedCount();
        emit artworksChanged();
    }

    void ArtItemsModel::removeSelectedArtworks(QList<int> &selectedIndices) {
        doRemoveItemsAtIndices(selectedIndices);
    }

    void ArtItemsModel::updateSelectedArtworks(const QList<int> &selectedIndices)
    {
        QList<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        QVector<int> roles;
        fillStandardRoles(roles);
        updateItemsInRanges(rangesToUpdate, roles);

        emit artworksChanged();
    }

    void ArtItemsModel::saveSelectedArtworks(const QList<int> &selectedIndices)
    {
        QList<ArtworkMetadata*> modifiedSelectedArtworks;

        foreach (int index, selectedIndices) {
            ArtworkMetadata *metadata = getArtwork(index);
            if (metadata != NULL &&
                    metadata->getIsSelected() &&
                    metadata->isModified()) {
                modifiedSelectedArtworks.append(metadata);
            }
        }

        // TODO: remove this two times copying
        m_CommandManager->setArtworksForIPTCProcessing(modifiedSelectedArtworks);
    }

    void ArtItemsModel::checkForWarnings(const QList<ArtworkMetadata *> selectedArtworks) {
        if (selectedArtworks.length() == 0) {
            emit needCheckItemsForWarnings(m_ArtworkList);
            qDebug() << "Checking all items for upload warnings...";
        } else {
            qDebug() << "Selected items: " << selectedArtworks.length();
            emit needCheckItemsForWarnings(selectedArtworks);
            qDebug() << "Checking selected items for upload warnings...";
        }
    }

    QObject *ArtItemsModel::getArtworkItself(int index) const {
        ArtworkMetadata *item = NULL;

        if (index >= 0 && index < m_ArtworkList.length()) {
            item = m_ArtworkList[index];
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    QString ArtItemsModel::retrieveImageSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return "-"; }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        QImageReader reader(metadata->getFilepath());
        QSize size = reader.size();
        QString sizeDescription = QString("W %1 x H %2").arg(size.width()).arg(size.height());
        return sizeDescription;
    }

    QString ArtItemsModel::retrieveFileSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return "-"; }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        QFileInfo fi(metadata->getFilepath());
        double size = fi.size(); // in bytes
        size /= 1024.0*1024.0;

        QString sizeDescription;
        if (size >= 1) {
            sizeDescription = QString::number(size, 'f', 2) + " MB";
        } else {
            size *= 1024;
            sizeDescription = QString::number(size, 'f', 2) + " KB";
        }

        return sizeDescription;
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
        bool needToUpdate = false;
        switch (role) {
        case EditArtworkDescriptionRole:
            needToUpdate = metadata->setDescription(value.toString());
            roleToUpdate = ArtworkDescriptionRole;
            break;
        case EditArtworkTitleRole:
            needToUpdate = metadata->setTitle(value.toString());
            roleToUpdate = ArtworkTitleRole;
            break;
        case EditIsSelectedRole:
            needToUpdate = metadata->setIsSelected(value.toBool());
            roleToUpdate = IsSelectedRole;
            break;
        default:
            return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << IsModifiedRole << roleToUpdate);
        }

        if (role == EditArtworkDescriptionRole ||
                role == EditArtworkTitleRole) {
            SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
            metadata->saveBackup(settingsModel);
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

    int ArtItemsModel::addLocalDirectories(const QList<QUrl> &directories)
    {
        qDebug() << "Adding local directories: " << directories;
        QStringList directoriesList;
        foreach (const QUrl &url, directories) {
            if (url.isLocalFile()) {
                directoriesList.append(url.toLocalFile());
            } else {
                directoriesList.append(url.path());
            }
        }

        int addedFilesCount = addDirectories(directoriesList);
        return addedFilesCount;
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

    void ArtItemsModel::appendMetadata(ArtworkMetadata *metadata)
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

    void ArtItemsModel::updateItemsAtIndices(const QList<int> &indices)
    {
        QList<QPair<int, int> > ranges;
        Helpers::indicesToRanges(indices, ranges);
        QVector<int> roles;
        fillStandardRoles(roles);
        updateItemsInRanges(ranges, roles);
    }

    int ArtItemsModel::addDirectories(const QStringList &directories) {
        int filesCount = 0;
        QStringList files;

        foreach (const QString &directory, directories) {
            doAddDirectory(directory, files);
        }

        if (files.count() > 0) {
            filesCount = addFiles(files);
        }

        return filesCount;
    }

    void ArtItemsModel::doAddDirectory(const QString &directory, QStringList &filesList) {
        QDir dir(directory);

        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

        QStringList items = dir.entryList();
        for (int i = 0; i < items.size(); ++i) {
            QString filepath = dir.filePath(items[i]);
            filesList.append(filepath);
        }
    }

    int ArtItemsModel::addFiles(const QStringList &rawFilenames)
    {
        QStringList filenames;

        foreach (const QString &filepath, rawFilenames) {
            QImageReader imageReader(filepath);

            if (imageReader.format() == "jpeg" ||
                    imageReader.format() == "tiff") {
                filenames.append(filepath);
            } else if (imageReader.format() == "png") {
                qDebug() << "PNG is unsupported file format";
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

    void ArtItemsModel::doCombineArtwork(int index)
    {
        if (index >= 0 && index < m_ArtworkList.length()) {
            QList<ArtItemInfo*> artworksList;

            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            metadata->setIsSelected(true);
            QModelIndex qmIndex = this->index(index);
            emit dataChanged(qmIndex, qmIndex, QVector<int>() << IsSelectedRole);

            ArtItemInfo *info = new ArtItemInfo(metadata, index);
            artworksList.append(info);

            m_CommandManager->combineArtworks(artworksList);
        }
    }

    QHash<int, QByteArray> ArtItemsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[ArtworkDescriptionRole] = "description";
        roles[EditArtworkDescriptionRole] = "editdescription";
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
        artworkRepository->removeFile(metadata->getFilepath(), metadata->getDirectory());

        if (metadata->getIsSelected()) {
            emit selectedArtworkRemoved();
        }

        delete metadata;
        m_ArtworkList.removeAt(row);
    }

    void ArtItemsModel::doRemoveItemsAtIndices(QList<int> &indicesToRemove)
    {
        qSort(indicesToRemove);
        QList<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        doRemoveItemsInRanges(rangesToRemove);
    }

    void ArtItemsModel::doRemoveItemsInRanges(const QList<QPair<int, int> > &rangesToRemove)
    {
        Commands::RemoveArtworksCommand *removeArtworksCommand = new Commands::RemoveArtworksCommand(rangesToRemove);
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

    void ArtItemsModel::fillStandardRoles(QVector<int> &roles) const
    {
        roles << ArtworkDescriptionRole << IsModifiedRole <<
                 ArtworkTitleRole << KeywordsCountRole;
    }
}
