/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
#include <QHash>
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
    ArtItemsModel::ArtItemsModel(QObject *parent):
        AbstractListModel(parent),
        Common::BaseEntity(),
        m_LastID(0)
    {}

    ArtItemsModel::~ArtItemsModel() {
        qDeleteAll(m_ArtworkList);
        qDeleteAll(m_FinalizationList);
    }

    ArtworkMetadata *ArtItemsModel::createMetadata(const QString &filepath) {
        int id = m_LastID++;
        return new ArtworkMetadata(filepath, id);
    }

    void ArtItemsModel::deleteAllItems() {
        qDebug() << "ArtItemsModel::deleteAllItems #";
        // should be called only from beforeDestruction() !
        // will not cause sync issues on shutdown if no items
        beginResetModel();
        qDeleteAll(m_ArtworkList);
        m_ArtworkList.clear();
        endResetModel();
    }

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
        AbstractListModel::updateItemsInRanges(rangesToUpdate, roles);
    }

    void ArtItemsModel::forceUnselectAllItems() const {
        int count = m_ArtworkList.count();
        for (int i = 0; i < count; ++i) {
            m_ArtworkList.at(i)->resetSelected();
        }
    }

    void ArtItemsModel::removeArtworksDirectory(int index) {
        qDebug() << "ArtItemsModel::removeArtworksDirectory #" << "Remove artworks directory at" << index;
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
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
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
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
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
                m_CommandManager->submitKeywordForSpellCheck(metadata, metadata->rowCount() - 1);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::pasteKeywords(int metadataIndex, const QStringList &keywords) {
        qDebug() << "ArtItemsModel::pasteKeywords #" << "item index" << metadataIndex;
        if (metadataIndex >= 0
                && metadataIndex < m_ArtworkList.length()
                && !keywords.empty()) {

            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

            QVector<ArtItemInfo*> artItemInfos;
            QVector<int> selectedIndices;

            getSelectedItemsIndices(selectedIndices);

            if (!metadata->getIsSelected()) {
                selectedIndices.append(metadataIndex);
                qDebug() << "ArtItemsModel::pasteKeywords #" << "item was not selected";
            }

            artItemInfos.reserve(selectedIndices.length());

            bool onlyOneKeyword = keywords.length() == 1;

            if (onlyOneKeyword) {
                qDebug() << "ArtItemsModel::pasteKeywords #" << "Pasting only one keyword";
            }

            foreach (int index, selectedIndices) {
                // only one keyword in added to the edit field
                // but not added as a new keyword
                if (onlyOneKeyword && (index == metadataIndex)) { continue; }

                ArtworkMetadata *metadata = m_ArtworkList.at(index);
                ArtItemInfo *item = new ArtItemInfo(metadata, index);
                artItemInfos.append(item);
            }

            Commands::PasteKeywordsCommand *pasteCommand = new Commands::PasteKeywordsCommand(artItemInfos, keywords);
            Commands::ICommandResult *result = m_CommandManager->processCommand(pasteCommand);
            Commands::PasteKeywordsCommandResult *pasteResult = static_cast<Commands::PasteKeywordsCommandResult*>(result);
            updateItems(pasteResult->m_IndicesToUpdate, QVector<int>() << IsModifiedRole << KeywordsCountRole);
            delete pasteResult;
        }
    }

    void ArtItemsModel::addSuggestedKeywords(int metadataIndex, const QStringList &keywords) {
        qDebug() << "ArtItemsModel::suggestKeywords #" << "item index" << metadataIndex;
        if (metadataIndex >= 0
                && metadataIndex < m_ArtworkList.length()
                && !keywords.empty()) {

            QVector<ArtItemInfo*> artItemInfos;

            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            ArtItemInfo *item = new ArtItemInfo(metadata, metadataIndex);
            artItemInfos.append(item);

            Commands::PasteKeywordsCommand *pasteCommand = new Commands::PasteKeywordsCommand(artItemInfos, keywords);
            Commands::ICommandResult *result = m_CommandManager->processCommand(pasteCommand);
            Commands::PasteKeywordsCommandResult *pasteResult = static_cast<Commands::PasteKeywordsCommandResult*>(result);
            updateItems(pasteResult->m_IndicesToUpdate, QVector<int>() << IsModifiedRole << KeywordsCountRole);
            delete pasteResult;
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
            m_CommandManager->submitKeywordsForWarningsCheck(metadata);
            // do not save metadata - give chance to restore from .xpks
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
        qDebug() << "ArtItemsModel::dropFiles #" << "Dropped" << urls.count() << "items(s)";
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

        qDebug() << "ArtItemsModel::dropFiles #" << "Added" << filesAddedCount << "files and" << directoriesAddedCount << "directories";

        return filesAddedCount + directoriesAddedCount;
    }

    void ArtItemsModel::setSelectedItemsSaved(const QVector<int> &selectedIndices) {
        qDebug() << "ArtItemsModel::setSelectedItemsSaved #" << "Setting selected" << selectedIndices.length() << "item(s) saved";
        foreach (int index, selectedIndices) {
            m_ArtworkList.at(index)->resetModified();
        }

        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, QVector<int>() << IsModifiedRole);

        updateModifiedCount();
        emit artworksChanged(false);
    }

    void ArtItemsModel::removeSelectedArtworks(QVector<int> &selectedIndices) {
        doRemoveItemsAtIndices(selectedIndices);

        if (m_ArtworkList.isEmpty()) {
            qDebug() << "ArtItemsModel::removeSelectedArtworks #" << "Clearing the finalization list";
            qDeleteAll(m_FinalizationList);
            m_FinalizationList.clear();
        }
    }

    void ArtItemsModel::updateSelectedArtworks(const QVector<int> &selectedIndices) {
        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        QVector<int> roles;
        fillStandardRoles(roles);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, roles);

        emit artworksChanged(false);
    }

    void ArtItemsModel::saveSelectedArtworks(const QVector<int> &selectedIndices, bool overwriteAll, bool useBackups) {
        QVector<ArtworkMetadata*> modifiedSelectedArtworks;
        int count = selectedIndices.count();
        modifiedSelectedArtworks.reserve(count/2);

        for (int i = 0; i < count; ++i) {
            int index = selectedIndices.at(i);
            ArtworkMetadata *metadata = getArtwork(index);
            if (metadata != NULL && metadata->getIsSelected()) {
                if (metadata->isModified() || overwriteAll) {
                    modifiedSelectedArtworks.append(metadata);
                }
            }
        }

        m_CommandManager->writeMetadata(modifiedSelectedArtworks, useBackups);
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
        QSize size;

        if (metadata->isInitialized()) {
            size = metadata->getImageSize();
        } else {
            QImageReader reader(metadata->getFilepath());
            size = reader.size();
            //metadata->setSize(size);
        }
        return size;
    }

    QString ArtItemsModel::retrieveFileSize(int metadataIndex) const {
        if (metadataIndex < 0 || metadataIndex >= m_ArtworkList.length()) { return QLatin1String("-"); }

        ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);

        double size = 0;

        if (metadata->isInitialized()) {
            size = metadata->getFileSize();
        } else {
            QFileInfo fi(metadata->getFilepath());
            size = fi.size(); // in bytes
        }

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
            metadata->notifySpellCheckResults(Common::SpellCheckDescription);
        }
    }

    void ArtItemsModel::initTitleHighlighting(int metadataIndex, QQuickTextDocument *document) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            SpellCheck::SpellCheckItemInfo *info = metadata->getSpellCheckInfo();
            info->createHighlighterForTitle(document->textDocument(), metadata);
            metadata->notifySpellCheckResults(Common::SpellCheckTitle);
        }
    }

    void ArtItemsModel::editKeyword(int metadataIndex, int keywordIndex, const QString &replacement) {
        if (0 <= metadataIndex && metadataIndex < m_ArtworkList.length()) {
            ArtworkMetadata *metadata = m_ArtworkList.at(metadataIndex);
            if (metadata->editKeyword(keywordIndex, replacement)) {
                m_CommandManager->submitKeywordForSpellCheck(metadata, keywordIndex);
                m_CommandManager->submitKeywordsForWarningsCheck(metadata);
                m_CommandManager->saveMetadata(metadata);
            }
        }
    }

    void ArtItemsModel::plainTextEdit(int metadataIndex, const QString &rawKeywords) {
        qDebug() << "ArtItemsModel::plainTextEdit #" << "Plain text edit for item" << metadataIndex;
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

            Commands::ICommandResult *result = m_CommandManager->processCommand(combinedEditCommand);
            Commands::CombinedEditCommandResult *combinedResult = static_cast<Commands::CombinedEditCommandResult*>(result);
            updateItemAtIndex(metadataIndex);

            delete combinedResult;
            delete itemInfo;
        }
    }

    void ArtItemsModel::detachVectorsFromSelected(const QVector<int> &selectedIndices) {
        qDebug() << "ArtItemsModel::detachVectorsFromSelected #" << selectedIndices.length() << "item(s) affected";
        foreach (int index, selectedIndices) {
            m_ArtworkList.at(index)->detachVector();
        }

        QVector<QPair<int, int> > rangesToUpdate;
        Helpers::indicesToRanges(selectedIndices, rangesToUpdate);
        AbstractListModel::updateItemsInRanges(rangesToUpdate, QVector<int>() << HasVectorAttachedRole);
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
        case HasVectorAttachedRole:
            return metadata->hasVectorAttached();
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

            if (role == EditArtworkDescriptionRole ||
                    role == EditArtworkTitleRole) {
                if (metadata->isInitialized()) {
                    m_CommandManager->saveMetadata(metadata);
                }
            }
        }

        return true;
    }

    int ArtItemsModel::addLocalArtworks(const QList<QUrl> &artworksPaths) {
        qDebug() << "ArtItemsModel::addLocalArtworks #" << artworksPaths;
        QStringList fileList;
        fileList.reserve(artworksPaths.length());

        foreach (const QUrl &url, artworksPaths) {
            fileList.append(url.toLocalFile());
        }

        int filesAddedCount = addFiles(fileList);
        return filesAddedCount;
    }

    int ArtItemsModel::addLocalDirectories(const QList<QUrl> &directories) {
        qDebug() << "ArtItemsModel::addLocalDirectories #" << directories;
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

    void ArtItemsModel::spellCheckErrorsChanged() {
        ArtworkMetadata *item = qobject_cast<ArtworkMetadata*>(sender());
        m_CommandManager->submitForWarningsCheck(item, Common::WarningsCheckSpelling);
    }

    void ArtItemsModel::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        AbstractListModel::removeItemsAtIndices(ranges);
        emit artworksChanged(true);
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

    void ArtItemsModel::removeArtworks(const QVector<QPair<int, int> > &ranges) {
        qDebug() << "ArtItemsModel::removeArtworks #" << ranges.count() << "range(s)";
        doRemoveItemsInRanges(ranges);
    }

    ArtworkMetadata *ArtItemsModel::getArtwork(int index) const {
        ArtworkMetadata *result = NULL;
        if (0 <= index && index < m_ArtworkList.length()) {
            result = m_ArtworkList.at(index);
        }

        return result;
    }

    void ArtItemsModel::raiseArtworksAdded(int imagesCount, int vectorsCount) {
        emit artworksAdded(imagesCount, vectorsCount);
        qDebug() << "ArtItemsModel::raiseArtworksAdded #" << imagesCount << "images" << vectorsCount << "vectors";
    }

    void ArtItemsModel::updateItemsAtIndices(const QVector<int> &indices) {
        QVector<QPair<int, int> > ranges;
        Helpers::indicesToRanges(indices, ranges);
        updateItemsInRanges(ranges);
    }

    void ArtItemsModel::updateItemsInRanges(const QVector<QPair<int, int> > &ranges) {
        QVector<int> roles;
        fillStandardRoles(roles);
        AbstractListModel::updateItemsInRanges(ranges, roles);
    }

    void ArtItemsModel::setAllItemsSelected(bool selected) {
        qDebug() << "ArtItemsModel::setAllItemsSelected #" << selected;
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

    int ArtItemsModel::attachVectors(const QHash<QString, QPair<QString, QString> > &vectorsPaths) const {
        qDebug() << "ArtItemsModel::attachVectors #" << vectorsPaths.size() << "vector(s)";

        int attachedVectors = 0;

        int size = m_ArtworkList.length();
        for (int i = 0; i < size; ++i) {
            ArtworkMetadata *metadata = m_ArtworkList.at(i);
            const QString &filepath = metadata->getFilepath();
            QFileInfo fi(filepath);

            const QString &directory = fi.absolutePath();
            if (vectorsPaths.contains(directory)) {
                const QPair<QString, QString> &pair = vectorsPaths.value(directory);

                const QString &filename = fi.baseName();
                if (QString::compare(filename, pair.first, Qt::CaseInsensitive) == 0) {
                    metadata->attachVector(pair.second);
                    attachedVectors++;
                }
            }
        }

        qInfo() << "ArtItemsModel::attachVectors #" << "Found matches to" << attachedVectors << "file(s)";

        return attachedVectors;
    }

    Common::IBasicArtwork *ArtItemsModel::getBasicArtwork(int index) const {
        Common::IBasicArtwork *result = NULL;
        if (0 <= index && index < m_ArtworkList.length()) {
            result = m_ArtworkList.at(index);
        }

        return result;
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

    int ArtItemsModel::addFiles(const QStringList &rawFilenames) {
        QStringList filenames, vectors;
        filenames.reserve(rawFilenames.length());
        vectors.reserve(rawFilenames.length());

        foreach (const QString &filepath, rawFilenames) {
            QImageReader imageReader(filepath);

            QString format = imageReader.format().toLower();

            if (format == QLatin1String("jpeg") ||
                    format == QLatin1String("tiff")) {
                filenames.append(filepath);
            } else if (format == QLatin1String("png")) {
                qWarning() << "ArtItemsModel::addFiles #" << "PNG is unsupported file format";
            } else {
                QFileInfo fi(filepath);
                QString suffix = fi.completeSuffix().toLower();
                if (suffix == QLatin1String("eps") ||
                        suffix == QLatin1String("ai")) {
                    vectors.append(filepath);
                }
            }
        }

        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
        bool autoFindVectors = settingsModel->getAutoFindVectors();

        Commands::AddArtworksCommand *addArtworksCommand = new Commands::AddArtworksCommand(filenames, vectors, autoFindVectors);
        Commands::ICommandResult *result = m_CommandManager->processCommand(addArtworksCommand);
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
        roles[HasVectorAttachedRole] = "hasvectorattached";
        return roles;
    }

    void ArtItemsModel::removeInnerItem(int row) {
        Q_ASSERT(row >= 0 && row < m_ArtworkList.length());
        ArtworkMetadata *metadata = m_ArtworkList.takeAt(row);
        ArtworksRepository *artworkRepository = m_CommandManager->getArtworksRepository();
        artworkRepository->removeFile(metadata->getFilepath(), metadata->getDirectory());

        if (metadata->getIsSelected()) {
            emit selectedArtworkRemoved();
        }

        if (metadata->release()) {
            delete metadata;
        } else {
            metadata->disconnect();
            qDebug() << "ArtItemsModel::removeInnerItem #" << "Metadata at index" << row << "is locked. Postponing destruction...";
            m_FinalizationList.append(metadata);
        }
    }

    void ArtItemsModel::doRemoveItemsAtIndices(QVector<int> &indicesToRemove) {
        qSort(indicesToRemove);
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        doRemoveItemsInRanges(rangesToRemove);
    }

    void ArtItemsModel::doRemoveItemsInRanges(const QVector<QPair<int, int> > &rangesToRemove) {
        Commands::RemoveArtworksCommand *removeArtworksCommand = new Commands::RemoveArtworksCommand(rangesToRemove);
        Commands::ICommandResult *result = m_CommandManager->processCommand(removeArtworksCommand);
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
                 ArtworkTitleRole << KeywordsCountRole << HasVectorAttachedRole;
    }
}
