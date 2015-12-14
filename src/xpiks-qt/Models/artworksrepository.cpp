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

#include "artworksrepository.h"
#include <QDir>
#include <QSet>
#include <QFileInfo>
#include <QRegExp>
#include "../Helpers/indiceshelper.h"

namespace Models {
    void ArtworksRepository::updateCountsForExistingDirectories() {
        emit dataChanged(index(0), index(rowCount() - 1), QVector<int>() << UsedImagesCountRole << IsSelectedRole);
    }

    void ArtworksRepository::cleanupEmptyDirectories() {
        int count = m_DirectoriesList.length();
        QVector<int> indicesToRemove;
        indicesToRemove.reserve(count);

        for (int i = 0; i < count; ++i) {
            const QString &directory = m_DirectoriesList[i];
            if (m_DirectoriesHash[directory] == 0) {
                indicesToRemove.append(i);
            }
        }

        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indicesToRemove, rangesToRemove);
        removeItemsAtIndices(rangesToRemove);
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
            QFileInfo fi(filepath);
            if (fi.exists()) {
                filteredDirectories.insert(fi.absolutePath());
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

    bool ArtworksRepository::accountFile(const QString &filepath) {
        bool wasModified = false;
        QString absolutePath;

        if (this->checkFileExists(filepath, absolutePath) &&
                !m_FilesSet.contains(filepath)) {

            int occurances = 0;
            if (!m_DirectoriesHash.contains(absolutePath)) {
                m_DirectoriesList.append(absolutePath);
                m_DirectoriesSelectedHash.insert(absolutePath, 0);
                emit artworksSourcesCountChanged();
            }
            else {
                occurances = m_DirectoriesHash[absolutePath];
            }

            m_FilesSet.insert(filepath);
            m_DirectoriesHash[absolutePath] = occurances + 1;
            wasModified = true;
        }

        return wasModified;
    }

    void ArtworksRepository::removeFile(const QString &filepath, const QString &fileDirectory) {
        if (m_DirectoriesHash.contains(fileDirectory)) {
            int occurances = m_DirectoriesHash[fileDirectory] - 1;
            int selectedCount = m_DirectoriesSelectedHash[fileDirectory] - 1;

            m_DirectoriesHash[fileDirectory] = occurances;
            m_DirectoriesSelectedHash[fileDirectory] = selectedCount;
        }

        m_FilesSet.remove(filepath);
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
            return QVariant(m_DirectoriesHash[directory]);
        case IsSelectedRole:
            return m_DirectoriesSelectedHash[directory] > 0;
        default:
            return QVariant();
        }
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
        QFileInfo fi(filename);
        bool exists = fi.exists();

        if (exists) {
            directory = fi.absolutePath();
        }

        return exists;
    }
}
