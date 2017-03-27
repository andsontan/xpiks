/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include "artworksrepository.h"
#include <QDir>
#include <QSet>
#include <QFileInfo>
#include <QRegExp>
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Commands/commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"

namespace Models {
    ArtworksRepository::ArtworksRepository(QObject *parent) :
        AbstractListModel(parent),
        m_LastUnavailableFilesCount(0),
        m_LastID(0)
    {
        QObject::connect(&m_FilesWatcher, SIGNAL(fileChanged(const QString &)),
                     this, SLOT(checkFileUnavailable(const QString &)));

        m_Timer.setInterval(4000); //4 sec
        m_Timer.setSingleShot(true); //single shot
        QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onAvailabilityTimer()));
    }

    void ArtworksRepository::updateCountsForExistingDirectories() {
        emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << UsedImagesCountRole << IsSelectedRole);
    }

    void ArtworksRepository::cleanupEmptyDirectories() {
        LOG_DEBUG << "#";
        size_t count = m_DirectoriesList.size();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve(count);

        for (size_t i = 0; i < count; ++i) {
            auto &directory = m_DirectoriesList[i];
            if (directory.m_FilesCount == 0) {
                indicesToRemove.append(i);
            }
        }

        if (!indicesToRemove.isEmpty()) {
            LOG_INFO << indicesToRemove.length() << "empty directory(ies)...";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);

            updateSelectedState();
        }
    }

    void ArtworksRepository::stopListeningToUnavailableFiles() {
        LOG_DEBUG << "#";

        QStringList files = m_FilesWatcher.files();
        if (!files.isEmpty()) {
            m_FilesWatcher.removePaths(files);
        }

        QStringList directories = m_FilesWatcher.directories();
        if (!directories.isEmpty()) {
            m_FilesWatcher.removePaths(directories);
        }
    }

    bool ArtworksRepository::beginAccountingFiles(const QStringList &items) {
        int count = getNewDirectoriesCount(items);
        bool shouldAccountFiles = count > 0;
        if (shouldAccountFiles) {
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + count - 1);
        }

        return shouldAccountFiles;
    }

    void ArtworksRepository::endAccountingFiles(bool filesWereAccounted) {
        if (filesWereAccounted) {
            endInsertRows();
        }
    }

    /*virtual */
    int ArtworksRepository::getNewDirectoriesCount(const QStringList &items) const {
        QSet<QString> filteredFiles;

        foreach (const QString &filepath, items) {
            if (!m_FilesSet.contains(filepath)) {
                filteredFiles.insert(filepath);
            }
        }

        QSet<QString> filteredDirectories;

        foreach (const QString &filepath, filteredFiles) {
            QString directory;
            if (checkFileExists(filepath, directory)) {
                filteredDirectories.insert(directory);
            }
        }

        int count = 0;
        size_t phony;
        foreach (const QString &directory, filteredDirectories) {
            if (!tryFindDirectory(directory, phony)) {
                count++;
            }
        }

        return count;
    }

    int ArtworksRepository::getNewFilesCount(const QStringList &items) const {
        int count = 0;
        QSet<QString> itemsSet = QSet<QString>::fromList(items);

        foreach (const QString &filepath, itemsSet) {
            if (!m_FilesSet.contains(filepath)) {
                count++;
            }
        }

        return count;
    }

    bool ArtworksRepository::isDirectoryIncluded(qint64 directoryID) const {
        bool isSelected = false;
        const size_t size = m_DirectoriesList.size();
        size_t index = m_DirectoryIdToIndex.value(directoryID, size);
        if (index < size) {
            isSelected = m_DirectoriesList[index].m_IsSelected;
        }

        return isSelected;
    }

    bool ArtworksRepository::accountFile(const QString &filepath, qint64 &directoryID) {
        bool wasModified = false;
        QString absolutePath;

        if (this->checkFileExists(filepath, absolutePath) &&
                !m_FilesSet.contains(filepath)) {

            int occurances = 0;
            size_t index;
            bool alreadyExists = tryFindDirectory(absolutePath, index);

            if (!alreadyExists) {
                qint64 id = generateNextID();
                LOG_INFO << "Adding new directory" << absolutePath << "with index" << m_DirectoriesList.size() << "and id" << id;
                m_DirectoriesList.emplace_back(absolutePath, id, 0, true);
                index = m_DirectoriesList.size() - 1;
                m_DirectoryIdToIndex[id] = index;
                directoryID = id;
                emit artworksSourcesCountChanged();
#ifdef CORE_TESTS
                if (m_CommandManager != nullptr)
#endif
                {
                    m_CommandManager->addToRecentDirectories(absolutePath);
                }
            } else {
                auto &item = m_DirectoriesList[index];
                occurances = item.m_FilesCount;
                directoryID = item.m_Id;
            }

            // watchFilePath(filepath);
            m_FilesSet.insert(filepath);
            m_DirectoriesList[index].m_FilesCount = occurances + 1;
            wasModified = true;
        }

        return wasModified;
    }

    void ArtworksRepository::accountVector(const QString &vectorPath) {
        watchFilePath(vectorPath);
    }

    bool ArtworksRepository::removeFile(const QString &filepath, qint64 directoryID) {
        bool result = false;

        if (m_FilesSet.contains(filepath)) {
            Q_ASSERT(m_DirectoryIdToIndex.contains(directoryID));
            auto existingIndex = m_DirectoryIdToIndex[directoryID];
            auto &item = m_DirectoriesList[existingIndex];
            item.m_FilesCount--;
            m_FilesWatcher.removePath(filepath);
            m_FilesSet.remove(filepath);
            result = true;
        }

        return result;
    }

    void ArtworksRepository::removeVector(const QString &vectorPath) {
        m_FilesWatcher.removePath(vectorPath);
    }

    void ArtworksRepository::purgeUnavailableFiles() {
        LOG_DEBUG << "#";
        m_UnavailableFiles.clear();
        m_LastUnavailableFilesCount = 0;
    }

    void ArtworksRepository::watchFilePaths(const QStringList &filePaths) {
#ifndef CORE_TESTS
        if (!filePaths.empty()) {
            m_FilesWatcher.addPaths(filePaths);
        }
#else
        Q_UNUSED(filePaths);
#endif
    }

    void ArtworksRepository::unwatchFilePaths(const QStringList &filePaths) {
#ifndef CORE_TESTS
        if (!filePaths.empty()) {
            m_FilesWatcher.removePaths(filePaths);
        }
#else
        Q_UNUSED(filePaths);
#endif
    }

    void ArtworksRepository::updateFilesCounts() {
        if (m_DirectoriesList.empty()) { return; }

        auto first = this->index(0);
        auto last = this->index(rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << UsedImagesCountRole);
    }

    void ArtworksRepository::updateSelectedState() {
        if (m_DirectoriesList.empty()) { return; }

        auto first = this->index(0);
        auto last = this->index(rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << IsSelectedRole);
    }

    void ArtworksRepository::watchFilePath(const QString &filepath) {
#ifndef CORE_TESTS
        m_FilesWatcher.addPath(filepath);
#else
        Q_UNUSED(filepath);
#endif
    }

    bool ArtworksRepository::isFileUnavailable(const QString &filepath) const {
        bool isUnavailable = false;

        if (m_UnavailableFiles.contains(filepath)) {
#if defined(CORE_TESTS) || defined(INTEGRATION_TESTS)
            {
#else
            if(!QFileInfo(filepath).exists()) {
#endif

                isUnavailable = true;
            }
        }

        return isUnavailable;
    }

#ifdef INTEGRATION_TESTS
    void ArtworksRepository::resetEverything() {
        m_DirectoriesList.clear();
        m_FilesSet.clear();
        m_DirectoryIdToIndex.clear();
    }
#endif

    int ArtworksRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return (int)m_DirectoriesList.size();
    }

    QVariant ArtworksRepository::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= (int)m_DirectoriesList.size())) {
            return QVariant();
        }

        auto &directory = m_DirectoriesList.at(index.row());

        switch (role) {
        case PathRole: {
            QDir dir(directory.m_AbsolutePath);
            return dir.dirName();
        }
        case UsedImagesCountRole:
            return directory.m_FilesCount;
        case IsSelectedRole:
            return !allAreSelected() && directory.m_IsSelected;
        default:
            return QVariant();
        }
    }

    void ArtworksRepository::selectDirectory(int row) {
        if ((row < 0) || (row >= (int)m_DirectoriesList.size())) {
            return;
        }

        auto &directory = m_DirectoriesList.at(row);

        bool oldValue = directory.m_IsSelected;
        bool newValue = !oldValue;
        if (changeSelectedState(row, newValue, oldValue)) {
            updateSelectedState();
#ifndef CORE_TESTS
            LOG_DEBUG << "Updating artworks";
            auto *filteredArtItemsModel = m_CommandManager->getFilteredArtItemsModel();
            Q_ASSERT(filteredArtItemsModel != NULL);
            filteredArtItemsModel->updateFilter();
#endif
        }
    }

    bool ArtworksRepository::setDirectorySelected(int index, bool value) {
        auto &directory = m_DirectoriesList[index];
        bool changed = directory.m_IsSelected != value;
        directory.m_IsSelected = value;

        return changed;
    }

    bool ArtworksRepository::changeSelectedState(int row, bool newValue, bool oldValue) {
        if (oldValue == newValue) { return false; }

        bool anySelectionChanged = false;

        const int count = m_DirectoriesList.size();
        if (count == 1) {
            Q_ASSERT(row == 0);
            anySelectionChanged = setDirectorySelected(row, true);
            return anySelectionChanged;
        }

        const int selectedCount = retrieveSelectedDirsCount();
        const bool allAreSelected = selectedCount == count;
        const bool wasSelected = (oldValue == true);
        const bool onlyOneSelected = (selectedCount == 1);
        const bool isUnselected = (newValue == false);

        if (allAreSelected && (count > 1)) {
            Q_ASSERT(newValue == false);
            unselectAllDirectories();
            setDirectorySelected(row, true);
            anySelectionChanged = true;
        } else if (onlyOneSelected && wasSelected && isUnselected) {
            selectAllDirectories();
            anySelectionChanged = true;
        } else {
            anySelectionChanged = setDirectorySelected(row, newValue);
        }

        return anySelectionChanged;
    }

    bool ArtworksRepository::setAllSelected(bool value) {
        bool anyChange = false;
        int size = m_DirectoriesList.size();

        for (int i = 0; i < size; i++) {
            if (setDirectorySelected(i, value)) {
                anyChange = true;
            }
        }

        if (anyChange) {
            updateSelectedState();
        }

        return anyChange;
    }

    size_t ArtworksRepository::retrieveSelectedDirsCount() const {
        size_t count = 0;

        for (auto &directory: m_DirectoriesList) {
            if (directory.m_IsSelected) {
                count++;
            }
        }

        return count;
    }

    bool ArtworksRepository::allAreSelected() const {
        bool anyUnselected = false;

        for (auto &item: m_DirectoriesList) {
            if (!item.m_IsSelected) {
                anyUnselected = true;
                break;
            }
        }

        return !anyUnselected;
    }

    bool ArtworksRepository::tryFindDirectory(const QString &directoryPath, size_t &index) const {
        bool found = false;
        const size_t size = m_DirectoriesList.size();

        for (size_t i = 0; i < size; ++i) {
            auto &item = m_DirectoriesList[i];
            if (item.m_AbsolutePath == directoryPath) {
                found = true;
                index = i;
                break;
            }
        }

        return found;
    }

    QHash<int, QByteArray> ArtworksRepository::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PathRole] = "path";
        roles[UsedImagesCountRole] = "usedimagescount";
        roles[IsSelectedRole] = "isselected";
        return roles;
    }

    /*virtual */
    bool ArtworksRepository::checkFileExists(const QString &filename, QString &directory) const {
        bool exists = false;
        QFileInfo fi(filename);

#ifndef CORE_TESTS
        exists = fi.exists();
#else
        exists = true;
#endif

        if (exists) {
            directory = fi.absolutePath();
        }

        return exists;
    }

    void ArtworksRepository::checkFileUnavailable(const QString &path) {
        LOG_INFO << "File changed:" << path;

        QFileInfo fi(path);
        if (!fi.exists()) {
            LOG_INFO << "File become unavailable:" << path;
            m_UnavailableFiles.insert(fi.absoluteFilePath());
            LOG_DEBUG << "Starting availability timer...";
            m_Timer.start();
        }
    }

    void ArtworksRepository::onAvailabilityTimer() {
        int currentUnavailableSize = m_UnavailableFiles.size();
        LOG_INFO << "Current:" << currentUnavailableSize << "Last:" << m_LastUnavailableFilesCount;

        if (currentUnavailableSize > m_LastUnavailableFilesCount) {
            m_LastUnavailableFilesCount = currentUnavailableSize;
            emit filesUnavailable();
        }
    }
}


