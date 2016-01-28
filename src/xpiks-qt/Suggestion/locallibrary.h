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

#ifndef LOCALLIBRARY_H
#define LOCALLIBRARY_H

#include <QHash>
#include <QString>
#include <QList>
#include <QStringList>
#include <QMutex>
#include <QFutureWatcher>
#include "libraryloaderworker.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Suggestion {
    class SuggestionArtwork;

    class LocalLibrary : public QObject
    {
        Q_OBJECT
    public:
        LocalLibrary();
        virtual ~LocalLibrary();

    public:
        void setLibraryPath(const QString &filename) { m_Filename = filename; }
        void addToLibrary(const QVector<Models::ArtworkMetadata *> artworksList);
        void swap(QHash<QString, QStringList> &hash);
        void saveToFile();
        void loadLibraryAsync();
        void searchArtworks(const QStringList &query, QVector<SuggestionArtwork *> &searchResults, int maxResults);
        void cleanupLocalLibraryAsync();
        void cleanupTrash();

    private slots:
        void artworksAdded();

    private:
        void saveLibraryAsync();
        void performAsync(Suggestion::LibraryLoaderWorker::LoadOption option);
        void doAddToLibrary(const QVector<Models::ArtworkMetadata *> artworksList);

    private:
        QFutureWatcher<void> *m_FutureWatcher;
        QHash<QString, QStringList> m_LocalArtworks;
        QMutex m_Mutex;
        QString m_Filename;
    };
}

#endif // LOCALLIBRARY_H
