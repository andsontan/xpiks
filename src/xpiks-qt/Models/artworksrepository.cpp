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
        m_NextID(0)
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
        int count = m_DirectoriesList.length();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve(count);

        for (int i = 0; i < count; ++i) {
            const QString &directory = m_DirectoriesList[i];
            int filesCount = m_DirectoriesHash[directory].m_FilesCount;
            if ( filesCount == 0) {
                indicesToRemove.append(i);
            }
        }

        if (!indicesToRemove.isEmpty()) {
            LOG_INFO << indicesToRemove.length() << "empty directory(ies)...";

            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
            removeItemsAtIndices(rangesToRemove);
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
        foreach (const QString &directory, filteredDirectories) {
            if (!m_DirectoriesHash.contains(directory)) {
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

    bool ArtworksRepository::isDirSelected(qint64 directoryID) const {
        bool result = false;

        for (const auto &element: m_DirectoriesHash) {
            if (element.m_Id == directoryID) {
                result = element.m_IsSelected;
                break;
            }
        }

        return result;
    }

    bool ArtworksRepository::accountFile(const QString &filepath, qint64 & directoryID) {
        bool wasModified = false;
        QString absolutePath;

        if (this->checkFileExists(filepath, absolutePath) &&
                !m_FilesSet.contains(filepath)) {

            int occurances = 0;
            QHash<QString, RepoDir>::iterator dirHashIterator = m_DirectoriesHash.find(absolutePath);

            if (dirHashIterator == m_DirectoriesHash.end()) {
                int id = m_NextID++;
                LOG_INFO << "Adding new directory" << absolutePath << "with index" << m_DirectoriesList.length() << "and id " << id;
                m_DirectoriesList.append(absolutePath);
                m_DirectoriesSelectedHash.insert(absolutePath, 0);
                dirHashIterator = m_DirectoriesHash.insert(absolutePath, {id, 0, true});
                emit artworksSourcesCountChanged();
#ifdef CORE_TESTS
                if (m_CommandManager != nullptr)
#endif
                {
                    m_CommandManager->addToRecentDirectories(absolutePath);
                }
            } else {
                occurances = dirHashIterator->m_FilesCount;
            }

            // watchFilePath(filepath);
            m_FilesSet.insert(filepath);
            dirHashIterator->m_FilesCount = occurances + 1;
            wasModified = true;
        }

        directoryID = m_DirectoriesHash[absolutePath].m_Id;

        return wasModified;
    }

    void ArtworksRepository::accountVector(const QString &vectorPath) {
        watchFilePath(vectorPath);
    }

    bool ArtworksRepository::removeFile(const QString &filepath, const QString &fileDirectory) {
        bool result = false;

        if (m_FilesSet.contains(filepath) && m_DirectoriesHash.contains(fileDirectory)) {
            int occurances = m_DirectoriesHash[fileDirectory].m_FilesCount - 1;
            int selectedCount = m_DirectoriesSelectedHash[fileDirectory] - 1;

            m_DirectoriesHash[fileDirectory].m_FilesCount = occurances;
            m_DirectoriesSelectedHash[fileDirectory] = selectedCount;
            m_FilesWatcher.removePath(filepath);
            m_FilesSet.remove(filepath);
            result = true;
        }

        return result;
    }

    void ArtworksRepository::removeVector(const QString &vectorPath) {
        m_FilesWatcher.removePath(vectorPath);
    }

    void ArtworksRepository::setFileSelected(const QString &filepath, bool selected) {
        QFileInfo fi(filepath);
        const QString absolutePath = fi.absolutePath();

        if (m_DirectoriesSelectedHash.contains(absolutePath)) {
            int plus = selected ? +1 : -1;
            m_DirectoriesSelectedHash[absolutePath] = m_DirectoriesSelectedHash[absolutePath] + plus;

            int directoryIndex = m_DirectoriesList.indexOf(QRegExp(absolutePath));
            QModelIndex index = this->index(directoryIndex);
            emit dataChanged(index, index, QVector<int>() << IsSelectedRole);
        }
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
        auto first = this->index(0);
        auto last = this->index(rowCount() - 1);
        emit dataChanged(first, last, QVector<int>() << UsedImagesCountRole);
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
        m_DirectoriesHash.clear();
        m_DirectoriesList.clear();
        m_DirectoriesSelectedHash.clear();
        m_FilesSet.clear();
    }
#endif

    int ArtworksRepository::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_DirectoriesList.count();
    }

    QVariant ArtworksRepository::data(const QModelIndex &index, int role) const {
        if (index.row() < 0 || index.row() >= m_DirectoriesList.count())
            return QVariant();

        const QString &directory = m_DirectoriesList.at(index.row());
        QDir dir(directory);

        switch (role) {
        case PathRole:
            return dir.dirName();
        case UsedImagesCountRole:
            return QVariant(m_DirectoriesHash[directory].m_FilesCount);
        case IsSelectedRole:
            return m_DirectoriesHash[directory].m_IsSelected;
        default:
            return QVariant();
        }
    }

    void ArtworksRepository::setDirSelected(int row) {

        if (row < 0 || row >= m_DirectoriesList.count()) {
            return;
        }

        const QString &directory = m_DirectoriesList.at(row);

        bool oldValue = m_DirectoriesHash[directory].m_IsSelected;
        bool newValue = !oldValue;
        setSelectedUnsafe(row, newValue, oldValue);
    }

    bool ArtworksRepository::setSelectedState(int index, bool newValue) {
        bool changed = false;
        auto &directory = m_DirectoriesList[index];
        bool oldValue = m_DirectoriesHash[directory].m_IsSelected;

        if (newValue != oldValue) {
            m_DirectoriesHash[directory].m_IsSelected = newValue;
            QModelIndex changedIndex = this->index(index);
            changed = true;
            emit dataChanged(changedIndex, changedIndex, QVector<int>() << IsSelectedRole);
        }

        return changed;
    }

    void ArtworksRepository::setSelectedUnsafe(int row, bool newValue, bool oldValue) {
        bool anySelectionChanged = false;
        int selectedCount = 0;

        if (oldValue == newValue) {
            return;
        }

        for (const auto &currentDirectory : m_DirectoriesList) {
            selectedCount += m_DirectoriesHash[currentDirectory].m_IsSelected ? 1 : 0;
        }

        int size = m_DirectoriesList.size();
        bool deselectAll = (size != 1) && (selectedCount == size);
        bool selectAll = (oldValue == true) && (size != 1) && (selectedCount == 1);

        if (deselectAll) {
            for (int i = 0; i < size; i++) {
                if (i == row) {
                    continue;
                }

                if (setSelectedState(i, false)) { anySelectionChanged = true; }
            }
        } else if (selectAll) {
            for (int i = 0; i < size; i++) {
                if (setSelectedState(i, true)) { anySelectionChanged = true; }
            }
        } else {
            anySelectionChanged = setSelectedState(row, newValue);
        }
#ifndef CORE_TESTS
        if (anySelectionChanged) {
            LOG_DEBUG << "Updating elements";
            auto *filteredArtItemsModel = m_CommandManager->getFilteredArtItemsModel();
            Q_ASSERT(filteredArtItemsModel != NULL);
            filteredArtItemsModel->updateFilter();
        }
#endif
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


