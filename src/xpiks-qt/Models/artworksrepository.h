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

#ifndef ARTWORKSDIRECTORIES_H
#define ARTWORKSDIRECTORIES_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QFileSystemWatcher>

#include <vector>

#include "../Common/abstractlistmodel.h"
#include "../Common/baseentity.h"

namespace Models {
    class ArtworksRepository : public Common::AbstractListModel, public Common::BaseEntity {
        Q_OBJECT
        Q_PROPERTY(int artworksSourcesCount READ getArtworksSourcesCount NOTIFY artworksSourcesCountChanged)
    public:
        ArtworksRepository(QObject *parent = 0);
        virtual ~ArtworksRepository() {}

    public:
        enum ArtworksRepository_Roles {
            PathRole = Qt::UserRole + 1,
            UsedImagesCountRole,
            IsSelectedRole
        };

    public:
        void updateCountsForExistingDirectories();
        void cleanupEmptyDirectories();
        void resetLastUnavailableFilesCount() { m_LastUnavailableFilesCount=0; }
        void stopListeningToUnavailableFiles();

    public:
        bool beginAccountingFiles(const QStringList &items);
        void endAccountingFiles(bool filesWereAccounted);

    public:
        virtual int getNewDirectoriesCount(const QStringList &items) const;
        int getNewFilesCount(const QStringList &items) const;
        int getArtworksSourcesCount() const { return (int)m_DirectoriesList.size(); }
        bool canPurgeUnavailableFiles() const { return m_UnavailableFiles.size() == m_LastUnavailableFilesCount; }
        bool isDirectoryIncluded(qint64 directoryID) const;

    signals:
        void artworksSourcesCountChanged();
        void fileChanged(const QString & path);
        void filesUnavailable();

#ifdef CORE_TESTS
    public:
        void removeItem(int index) { removeInnerItem(index); }
    protected:
        void insertIntoUnavailable(const QString &value) { m_UnavailableFiles.insert(value); }
        const QSet<QString> &getFilesSet() const { return m_FilesSet; }
#endif

    private slots:
        void checkFileUnavailable(const QString & path);
        void onAvailabilityTimer();

    public:
        bool accountFile(const QString &filepath, qint64 &directoryID);
        void accountVector(const QString &vectorPath);
        bool removeFile(const QString &filepath, qint64 directoryID);
        void removeVector(const QString &vectorPath);
        void purgeUnavailableFiles();
        void watchFilePaths(const QStringList &filePaths);
        void unwatchFilePaths(const QStringList &filePaths);
        void updateFilesCounts();
        void updateSelectedState();

    private:
        void watchFilePath(const QString &filepath);
        qint64 generateNextID() { qint64 id = m_LastID; m_LastID++; return id; }

    public:
        const QString &getDirectory(int index) const { return m_DirectoriesList[index].m_AbsolutePath; }
#ifdef CORE_TESTS
        int getFilesCountForDirectory(const QString &directory) const { size_t index; tryFindDirectory(directory, index); return m_DirectoriesList[index].m_FilesCount; }
        int getFilesCountForDirectory(int index) const { return m_DirectoriesList[index].m_FilesCount; }
#endif
        bool isFileUnavailable(const QString &filepath) const;

#ifdef INTEGRATION_TESTS
        void resetEverything();
#endif

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    public:
        Q_INVOKABLE void selectDirectory(int row);

    protected:
        virtual QHash<int, QByteArray> roleNames() const override;

    protected:
        virtual void removeInnerItem(int index) override {
            auto &directoryToRemove = m_DirectoriesList.at(index);
            qint64 idToRemove = directoryToRemove.m_Id;
            const bool oldIsSelected = directoryToRemove.m_IsSelected;
            const bool newIsSelected = false; // unselect folder to be deleted
            changeSelectedState(index, newIsSelected, oldIsSelected);
            m_DirectoriesList.erase(m_DirectoriesList.begin() + index);
            m_DirectoryIdToIndex.remove(idToRemove);
            emit artworksSourcesCountChanged();
        }

        virtual bool checkFileExists(const QString &filename, QString &directory) const;

    private:
        bool setDirectorySelected(int index, bool value);
        bool changeSelectedState(int row, bool newValue, bool oldValue);
        bool unselectAllDirectories() { return setAllSelected(false); }
        bool selectAllDirectories() { return setAllSelected(true); }
        bool setAllSelected(bool value);
        size_t retrieveSelectedDirsCount() const;
        bool allAreSelected() const;
        bool tryFindDirectory(const QString &directoryPath, size_t &index) const;

    private:
        struct RepoDir {
            RepoDir (QString absolutePath, qint64 id, int count, bool selected):
                m_AbsolutePath(absolutePath), m_Id(id), m_FilesCount(count), m_IsSelected(selected)
            { }
            RepoDir() = default;

            QString m_AbsolutePath = QString("");
            qint64 m_Id = 0;
            int m_FilesCount = 0;
            bool m_IsSelected = true;
        };

    private:
        std::vector<RepoDir> m_DirectoriesList;
        QHash<qint64, size_t> m_DirectoryIdToIndex;
        QSet<QString> m_FilesSet;
        QFileSystemWatcher m_FilesWatcher;
        QTimer m_Timer;
        QSet<QString> m_UnavailableFiles;
        int m_LastUnavailableFilesCount;
        qint64 m_LastID;
    };
}

#endif // ARTWORKSDIRECTORIES_H
