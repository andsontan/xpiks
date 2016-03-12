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

#ifndef ARTWORKSDIRECTORIES_H
#define ARTWORKSDIRECTORIES_H

#include <QAbstractListModel>
#include <QStringList>
#include <QList>
#include <QPair>
#include <QSet>
#include <QTimer>
#include <QMutex>
#include "abstractlistmodel.h"
#include "../Common/baseentity.h"
#include <QFileSystemWatcher>

namespace Models {
    class ArtworksRepository : public AbstractListModel, public Common::BaseEntity {
        Q_OBJECT
        Q_PROPERTY(int artworksSourcesCount READ getArtworksSourcesCount NOTIFY artworksSourcesCountChanged)
    public:
        ArtworksRepository(QObject *parent = 0) :
            AbstractListModel(parent)
        {
            QObject::connect(&m_FilesWatcher, SIGNAL(fileChanged(const QString &)),
                         this, SLOT(checkfileUnavailable(const QString &) ) );
            m_Timer.setInterval(1000); //1 sec
            m_Timer.setSingleShot(true); //single shot
            QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
            m_Timer.start();

        }

        ArtworksRepository(const ArtworksRepository &copy):
            AbstractListModel(),
            Common::BaseEntity(),
            m_DirectoriesList(copy.m_DirectoriesList),
            m_DirectoriesHash(copy.m_DirectoriesHash),
            m_FilesSet(copy.m_FilesSet)
        {

            m_FilesWatcher.addPaths(copy.m_DirectoriesList);
            QObject::connect(&m_FilesWatcher, SIGNAL(fileChanged(const QString &)),
                          this, SLOT(checkfileUnavailable(const QString &) ) );
            m_Timer.setInterval(1000); //1 sec
            m_Timer.setSingleShot(true); //single shot
            QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
        }

        virtual ~ArtworksRepository() {}

    public:
        enum ArtworksDirectoriesRoles {
            PathRole = Qt::UserRole + 1,
            UsedImagesCountRole,
            IsSelectedRole
        };

    public:
        void updateCountsForExistingDirectories();
        void cleanupEmptyDirectories();

    public:
        bool beginAccountingFiles(const QStringList &items);
        void endAccountingFiles(bool filesWereAccounted);

    public:
        virtual int getNewDirectoriesCount(const QStringList &items) const;
        int getNewFilesCount(const QStringList &items) const;
        int getArtworksSourcesCount() const { return m_DirectoriesList.length(); }

    signals:
        void artworksSourcesCountChanged();
        void fileChanged(const QString & path);
        void fileUnavailable();

    public slots:
        void fileSelectedChanged(const QString &filepath, bool isSelected) { setFileSelected(filepath, isSelected); }
        void checkfileUnavailable(const QString & path);
        void onTimer();
    public:
        bool accountFile(const QString &filepath);
        bool removeFile(const QString &filepath, const QString &fileDirectory);
        void setFileSelected(const QString &filepath, bool selected);

        const QString &getDirectory(int index) const { return m_DirectoriesList[index]; }
#ifdef TESTS
        int getFilesCountForDirectory(const QString &directory) const { return m_DirectoriesHash[directory]; }
#endif
        bool isFileUnavailable(const QString &filepath) const { return (m_UnavailableFiles.find(filepath) != m_UnavailableFiles.end()); }

#ifdef INTEGRATION_TESTS
        void resetEverything();
#endif

    public:
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        void removeFromDeletedList(const QString &filepath);
    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        void removeInnerItem(int index) {
            QString directoryToRemove = m_DirectoriesList.takeAt(index);
            m_DirectoriesSelectedHash.remove(directoryToRemove);
            m_DirectoriesHash.remove(directoryToRemove);
            emit artworksSourcesCountChanged();
        }

        void removeFilesAndEmitSignal(int num){
            size_t k=num;
            QSet<QString>::iterator begin = m_FilesSet.begin();
            QSet<QString>::iterator end = m_FilesSet.end();

            for (QSet<QString>::iterator it = begin; ((it!=end) && (k--)); ++it){
                m_UnavailableFiles.insert(*it);
            }
            emit fileUnavailable();
        }

        virtual bool checkFileExists(const QString &filename, QString &directory) const;

    private:
        QStringList m_DirectoriesList;
        QHash<QString, int> m_DirectoriesHash;
        QSet<QString> m_FilesSet;
        QHash<QString, int> m_DirectoriesSelectedHash;
        QFileSystemWatcher  m_FilesWatcher;
        QTimer m_Timer;
        QSet<QString> m_UnavailableFiles;
        QMutex m_Mutex;
    };
}

#endif // ARTWORKSDIRECTORIES_H
