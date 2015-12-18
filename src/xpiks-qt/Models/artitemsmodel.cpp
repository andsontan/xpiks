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
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Common/flags.h"
#include "../Commands/combinededitcommand.h"

namespace Models {
    ArtItemsModel::~ArtItemsModel() {
        qDeleteAll(m_ArtworkList);
        m_ArtworkList.clear();
    }

    ArtworkMetadata *ArtItemsModel::createMetadata(const QString &filepath) const { return new ArtworkMetadata(filepath); }

    int ArtItemsModel::getModifiedArtworksCount() {
        int modifiedCount = 0;
        int length = m_ArtworkList.length();

        for (int i = 0; i < length; ++i) {
            if (m_ArtworkList.at(i)->isModified()) {
                modifiedCount++;
            }
        }

        return modifiedCount;
    }

    void ArtItemsModel::updateItems(const QVector<int> &indices, const QVector<int> &roles) {
        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(indices, rangesToUpdate);
        updateItemsInRanges(rangesToUpdate, roles);
    }

    void ArtItemsModel::forceUnselectAllItems() const {
        int count = m_ArtworkList.count();
        for (int i = 0; i < count; ++i) {
            m_ArtworkList.at(i)->resetSelected();
        }
    }

    void ArtItemsModel::updateLastN(int N) {
        qDebug() << "Updating last" << N << "item(s)";
        int length = m_ArtworkList.length();

        if (0 < N && N <= length) {
            int start = length - N;
            int end = length - 1;

            QVector<QPair<int, int> > ranges;
            ranges << qMakePair(start, end);

            QVector<int> roles;
            fillStandardRoles(roles);

            updateItemsInRanges(ranges, roles);
        }
    }

    void ArtItemsModel::removeArtworksDirectory(int index) {
        qDebug() << "Remove artworks directory at" << index;
        const QString &directory = m_CommandManager->getArtworksRepository()->getDirectory(index);
        QDir dir(directory);
        QString directoryAbsolutePath = dir.absolutePath();

        QVector<int> indicesToRemove;
        int count = m_ArtworkList.length();
        indicesToRemove.reserve(count);

        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            if (metadata->isInDirectory(directoryAbsolutePath)) {
                indicesToRemove.append(i);
            }
        }

        doRemoveItemsAtIndices(indicesToRemove);

        emit modifiedArtworksCountChanged();
    }

    void ArtItemsModel::removeKeywordAt(int metadataIndex, int keywordIndex) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeKeywordAt(keywordIndex)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::removeLastKeyword(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            if (metadata->removeLastKeyword()) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::appendKeyword(int metadataIndex, const QString &keyword) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->appendKeyword(keyword)) {
                QModelIndex index = this->index(metadataIndex);
                emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
                m_CommandManager->submitForSpellCheck(metadata, metadata->rowCount() - 1);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::pasteKeywords(int metadataIndex, const QStringList &keywords) {
        qDebug() << "Pasting keywords on item" << metadataIndex;
        if (metadataIndex >= 0
                && metadataIndex < m_ArtworkList.length()
                && !keywords.empty()) {

            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            QVector<ArtItemInfo*> artItemInfos;
            QVector<int> selectedIndices;

            getSelectedItemsIndices(selectedIndices);

            if (!metadata->getIsSelected()) {
                selectedIndices.append(metadataIndex);
                qDebug() << "Pasting keywords: item was not selected";
            }

            artItemInfos.reserve(selectedIndices.length());

            foreach (int index, selectedIndices) {
                ArtworkMetadata *metadata = m_ArtworkList.at(index);
                ArtItemInfo *item = new ArtItemInfo(metadata, index);
                artItemInfos.append(item);
            }

            Commands::PasteKeywordsCommand *pasteCommand = new Commands::PasteKeywordsCommand(artItemInfos, keywords);
            Commands::CommandResult *result = m_CommandManager->processCommand(pasteCommand);
            delete result;

            QModelIndex index = this->index(metadataIndex);
            emit dataChanged(index, index, QVector<int>() << IsModifiedRole << KeywordsCountRole);
        }
    }

    void ArtItemsModel::suggestCorrections(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            int flags = 0;
            Common::SetFlag(flags, Common::CorrectDescription);
            Common::SetFlag(flags, Common::CorrectTitle);
            Common::SetFlag(flags, Common::CorrectKeywords);
            m_CommandManager->setupSpellCheckSuggestions(m_ArtworkList.at(metadataIndex), metadataIndex, flags);
        }
    }

    void ArtItemsModel::clearKeywords(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            metadata->clearKeywords();
            // do not save metadata - give change to restore from .xpks
            //m_CommandManager->saveMetadata(metadata);
        }
    }

    void ArtItemsModel::backupItem(int metadataIndex) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            m_CommandManager->saveMetadata(metadata);
        }
    }

    int ArtItemsModel::dropFiles(const QList<QUrl> &urls) {
        qDebug() << "Dropped" << urls.count() << "items(s)";
        QList<QUrl> directories, files;
        directories.reserve(urls.count()/2);
        files.reserve(urls.count());

        foreach(const QUrl &url, urls) {
            bool isDirectory = QDir(url.toLocalFile()).exists();
            if (isDirectory) {
                directories.append(url);
            } else {
                files.append(url);
            }
        }

        int filesAddedCount = addLocalArtworks(files);
        int directoriesAddedCount = addLocalDirectories(directories);

        qDebug() << "Added" << filesAddedCount << "files and" << directoriesAddedCount << "directories";

        return filesAddedCount + directoriesAddedCount;
    }

    void ArtItemsModel::setSelectedItemsSaved(const QVector<int> &selectedIndices) {
        qDebug() << "Setting selected" << selectedIndices.length() << "item(s) saved";
        foreach (int index, selectedIndices) {
            m_ArtworkList.at(index)->resetModified();
        }

        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        updateItemsInRanges(rangesToUpdate, QVector<int>() << IsModifiedRole);

        updateModifiedCount();
        emit artworksChanged();
    }

    void ArtItemsModel::removeSelectedArtworks(QVector<int> &selectedIndices) {
        doRemoveItemsAtIndices(selectedIndices);
    }

    void ArtItemsModel::updateSelectedArtworks(const QVector<int> &selectedIndices) {
        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        QVector<int> roles;
        fillStandardRoles(roles);
        updateItemsInRanges(rangesToUpdate, roles);

        emit artworksChanged();
    }

    void ArtItemsModel::saveSelectedArtworks(const QVector<int> &selectedIndices) {
        QVector<ArtworkMetadata*> modifiedSelectedArtworks;
        int count = selectedIndices.count();
        modifiedSelectedArtworks.reserve(count/2);

        for (int i = 0; i < count; ++i) {
            int index = selectedIndices.at(i);
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

    QObject *ArtItemsModel::getArtworkItself(int index) const {
        ArtworkMetadata *item = NULL;

        if (0 <= index && index < m_ArtworkList.length()) {
            item = m_ArtworkList.at(index);
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    QSize ArtItemsModel::retrieveImageSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return QSize(); }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        QSize size = metadata->getSize();
        return size;
    }

    QString ArtItemsModel::retrieveFileSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return QLatin1String("-"); }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        QFileInfo fi(metadata->getFilepath());
        double size = fi.size(); // in bytes
        size /= 1024.0*1024.0;

        QString sizeDescription;
        if (size >= 1) {
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" MB");
        } else {
            size *= 1024;
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" KB");
        }

        return sizeDescription;
    }

    QString ArtItemsModel::getArtworkFilepath(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return QLatin1String(""); }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
        return metadata->getFilepath();
    }

    int ArtItemsModel::addRecentDirectory(const QString &directory) {
        int filesAdded = addDirectories(QStringList() << directory);
        return filesAdded;
    }

    void ArtItemsModel::initDescriptionHighlighting(int metadataIndex, QQuickTextDocument *document) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            SpellCheck::SpellCheckItemInfo *info = metadata->getSpellCheckInfo();
            info->createHighlighterForDescription(document->textDocument(), metadata);
            metadata->notifySpellCheckResults();
        }
    }

    void ArtItemsModel::initTitleHighlighting(int metadataIndex, QQuickTextDocument *document) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            SpellCheck::SpellCheckItemInfo *info = metadata->getSpellCheckInfo();
            info->createHighlighterForTitle(document->textDocument(), metadata);
            metadata->notifySpellCheckResults();
        }
    }

    void ArtItemsModel::editKeyword(int metadataIndex, int keywordIndex, const QString &replacement) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->editKeyword(keywordIndex, replacement)) {
                m_CommandManager->submitForSpellCheck(metadata, keywordIndex);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::plainTextEdit(int metadataIndex, const QString &rawKeywords) {
        qDebug() << "Plain text edit for item" << metadataIndex;
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            ArtItemInfo *itemInfo = new ArtItemInfo(metadata, metadataIndex);

            QStringList keywords = rawKeywords.trimmed().split(QChar(','), QString::SkipEmptyParts);

            int flags = 0;
            Common::SetFlag(flags, Common::EditKeywords);
            Commands::CombinedEditCommand *combinedEditCommand = new Commands::CombinedEditCommand(
                        flags,
                        QVector<ArtItemInfo*>() << itemInfo,
                        "", "",
                        keywords);

            Commands::CommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
            Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
            updateItemAtIndex(metadataIndex);

            delete combinedResult;
            delete itemInfo;
        }
    }

    int ArtItemsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_ArtworkList.count();
    }

    QVariant ArtItemsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_ArtworkList.length()) {
            return QVariant();
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(row);
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

    Qt::ItemFlags ArtItemsModel::flags(const QModelIndex &index) const {
        int row = index.row();
        if (row < 0 || row >= m_ArtworkList.length()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool ArtItemsModel::setData(const QModelIndex &index, const QVariant &value, int role) {
        int row = index.row();
        if (row < 0 || row >= m_ArtworkList.length()) {
            return false;
        }

        ArtworkMetadata *metadata = m_ArtworkList.at(row);
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
            if (metadata->isInitialized()) {
                m_CommandManager->saveMetadata(metadata);
            }
        }

        return true;
    }

    int ArtItemsModel::addLocalArtworks(const QList<QUrl> &artworksPaths) {
        qDebug() << "Adding local files:" << artworksPaths;
        QStringList fileList;
        fileList.reserve(artworksPaths.length());

        foreach (const QUrl &url, artworksPaths) {
            fileList.append(url.toLocalFile());
        }

        int filesAddedCount = addFiles(fileList);
        return filesAddedCount;
    }

    int ArtItemsModel::addLocalDirectories(const QList<QUrl> &directories) {
        qDebug() << "Adding local directories:" << directories;
        QStringList directoriesList;
        directoriesList.reserve(directories.length());

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

    void ArtItemsModel::beginAccountingFiles(int filesCount) {
        int rowsCount = rowCount();
        beginInsertRows(QModelIndex(), rowsCount, rowsCount + filesCount - 1);
    }

    void ArtItemsModel::beginAccountingFiles(int start, int end) {
        beginInsertRows(QModelIndex(), start, end);
    }

    void ArtItemsModel::endAccountingFiles() {
        endInsertRows();
    }

    void ArtItemsModel::insertArtwork(int index, ArtworkMetadata *metadata) {
        Q_ASSERT(index >= 0 && index <= m_ArtworkList.length());
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.insert(index, metadata);
    }

    void ArtItemsModel::appendMetadata(ArtworkMetadata *metadata) {
        Q_ASSERT(metadata != NULL);
        m_ArtworkList.append(metadata);
    }

    ArtworkMetadata *ArtItemsModel::getArtwork(int index) const {
        ArtworkMetadata *result = NULL;
        if (0 <= index && index < m_ArtworkList.length()) {
            result = m_ArtworkList.at(index);
        }

        return result;
    }

    void ArtItemsModel::updateItemsAtIndices(const QVector<int> &indices) {
        QVector<QPair<int, int> > ranges;
        Helpers::indicesToRanges(indices, ranges);
        QVector<int> roles;
        fillStandardRoles(roles);
        updateItemsInRanges(ranges, roles);
    }

    void ArtItemsModel::setAllItemsSelected(bool selected)
    {
        qDebug() << "Setting all items selected (" << selected << ")";
        int length = m_ArtworkList.length();
        for (int i = 0; i < length; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            metadata->setIsSelected(selected);
        }

        if (length > 0) {
            QModelIndex startIndex = index(0);
            QModelIndex endIndex = index(length - 1);
            emit dataChanged(startIndex, endIndex, QVector<int>() << IsSelectedRole);
        }
    }

    void ArtItemsModel::updateItemAtIndex(int metadataIndex) {
        QVector<int> roles;
        fillStandardRoles(roles);
        QModelIndex topLeft = this->index(metadataIndex);
        QModelIndex bottomRight = this->index(metadataIndex);
        emit dataChanged(topLeft, bottomRight, roles);
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

        QFileInfoList items = dir.entryInfoList();
        int size = items.size();
        filesList.reserve(size);
        for (int i = 0; i < size; ++i) {
            QString filepath = items.at(i).absoluteFilePath();
            filesList.append(filepath);
        }
    }

    int ArtItemsModel::addFiles(const QStringList &rawFilenames)
    {
        QStringList filenames;
        filenames.reserve(rawFilenames.length());

        foreach (const QString &filepath, rawFilenames) {
            QImageReader imageReader(filepath);

            QString format = imageReader.format().toLower();

            if (format == QLatin1String("jpeg") ||
                    format == QLatin1String("tiff")) {
                filenames.append(filepath);
            } else if (format == QLatin1String("png")) {
                qWarning() << "PNG is unsupported file format";
            }
        }

        Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames);
        Commands::CommandResult *result = m_CommandManager->processCommand(addArtworksCommand);
        Commands::AddArtworksCommandResult *addArtworksResult = static_cast<Commands::AddArtworksCommandResult*>(result);
        int newFilesCount = addArtworksResult->m_NewFilesAdded;
        delete result;

        return newFilesCount;
    }

    void ArtItemsModel::getSelectedArtworks(QVector<ArtworkMetadata *> &selectedArtworks) const {
        int count = m_ArtworkList.length();
        for (int i = 0; i < count; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            if (metadata->getIsSelected()) {
                selectedArtworks.append(metadata);
            }
        }
    }

    void ArtItemsModel::doCombineArtwork(int index) {
        if (0 <= index && index < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(index);
            //QModelIndex qmIndex = this->index(index);
            //emit dataChanged(qmIndex, qmIndex, QVector<int>() << IsSelectedRole);

            ArtItemInfo *info = new ArtItemInfo(metadata, index);
            m_CommandManager->combineArtwork(info);
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

    void ArtItemsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < m_ArtworkList.length());
        ArtworkMetadata *metadata = m_ArtworkList.at(row);
        ArtworksRepository *artworkRepository = m_CommandManager->getArtworksRepository();
        artworkRepository->removeFile(metadata->getFilepath(), metadata->getDirectory());

        if (metadata->getIsSelected()) {
            emit selectedArtworkRemoved();
        }

        delete metadata;
        m_ArtworkList.removeAt(row);
    }

    void ArtItemsModel::doRemoveItemsAtIndices(QVector<int> &indicesToRemove) {
        qSort(indicesToRemove);
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        doRemoveItemsInRanges(rangesToRemove);
    }

    void ArtItemsModel::doRemoveItemsInRanges(const QVector<QPair<int, int> > &rangesToRemove) {
        Commands::RemoveArtworksCommand *removeArtworksCommand = new Commands::RemoveArtworksCommand(rangesToRemove);
        Commands::CommandResult *result = m_CommandManager->processCommand(removeArtworksCommand);
        delete result;
    }

    void ArtItemsModel::getSelectedItemsIndices(QVector<int> &indices) {
        int count = m_ArtworkList.length();
        indices.reserve(count / 3);
        for (int i = 0; i < count; ++i) {
            if (m_ArtworkList.at(i)->getIsSelected()) {
                indices.append(i);
            }
        }
    }

    void ArtItemsModel::fillStandardRoles(QVector<int> &roles) const {
        roles << ArtworkDescriptionRole << IsModifiedRole <<
                 ArtworkTitleRole << KeywordsCountRole;
    }
}
