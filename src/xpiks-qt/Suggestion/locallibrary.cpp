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

#include <QtAlgorithms>
#include <QFile>
#include <QDataStream>
#include <QtConcurrent>
#include <QFutureWatcher>
#include "locallibrary.h"
#include "../Models/artworkmetadata.h"
#include "libraryloaderworker.h"
#include "suggestionartwork.h"
#include "../Common/defines.h"
#include "../Common/basickeywordsmodel.h"
#include "../Models/imageartwork.h"

namespace Suggestion {
    QDataStream &operator<<(QDataStream &out, const LocalArtworkData &v) {
        out << v.m_ArtworkType << v.m_Title << v.m_Description << v.m_Keywords << v.m_CreationTime << v.m_ReservedString << v.m_ReservedInt;
        return out;
    }

    QDataStream &operator>>(QDataStream &in, LocalArtworkData &v) {
        in >> v.m_ArtworkType >> v.m_Title >> v.m_Description >> v.m_Keywords >> v.m_CreationTime >> v.m_ReservedString >> v.m_ReservedInt;
        return in;
    }

    LocalLibrary::LocalLibrary():
        QObject(),
        m_FutureWatcher(NULL)
    {
        m_FutureWatcher = new QFutureWatcher<void>(this);
        QObject::connect(m_FutureWatcher, SIGNAL(finished()), this, SLOT(artworksAdded()));
    }

    LocalLibrary::~LocalLibrary() {
        delete m_FutureWatcher;
    }

    void LocalLibrary::addToLibrary(const QVector<Models::ArtworkMetadata *> artworksList) {
        // adding to library will be complicated in future
        // so always do it in the background
#ifndef INTEGRATION_TESTS
        m_FutureWatcher->setFuture(QtConcurrent::run(this, &LocalLibrary::doAddToLibrary, artworksList));
#else
        doAddToLibrary(artworksList);
#endif
    }

    void LocalLibrary::swap(QHash<QString, LocalArtworkData> &hash) {
        QMutexLocker locker(&m_Mutex);
        m_LocalArtworks.swap(hash);
        LOG_DEBUG << "swapped with read from db.";
    }

    void LocalLibrary::saveToFile() {
        QFile file(m_Filename);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);   // write the data

            m_Mutex.lock();
            {
                out << m_LocalArtworks;
            }
            m_Mutex.unlock();

            file.close();

            LOG_DEBUG << "saved to" << m_Filename;
        }
    }

    void LocalLibrary::loadLibraryAsync() {
        performAsync(LibraryLoaderWorker::Load);
    }

    void LocalLibrary::searchArtworks(const QStringList &query,
                                      std::vector<std::shared_ptr<SuggestionArtwork> > &searchResults,
                                      size_t maxResults) {
        LOG_DEBUG << "max results" << maxResults;
        QMutexLocker locker(&m_Mutex);

        QHashIterator<QString, LocalArtworkData> i(m_LocalArtworks);
        QVector<QPair<QDateTime, QString> > results;

        while (i.hasNext()) {
            i.next();

            bool anyError = false;

            auto &localData = i.value();

            const QStringList &keywords = localData.m_Keywords;
            foreach (const QString &searchTerm, query) {
                bool containsTerm = false;

                foreach (const QString &keyword, keywords) {
                    if (keyword.contains(searchTerm, Qt::CaseInsensitive)) {
                        containsTerm = true;
                        break;
                    }
                }

                if (localData.m_Title.contains(searchTerm, Qt::CaseInsensitive)) {
                    containsTerm = true;
                    break;
                }

                if (localData.m_Description.contains(searchTerm, Qt::CaseInsensitive)) {
                    containsTerm = true;
                    break;
                }

                if (!containsTerm) {
                    anyError = true;
                    break;
                }
            }

            if (!anyError) {
                if (QFileInfo(i.key()).exists()) {
                    results.append(qMakePair(localData.m_CreationTime, i.key()));

                    if (searchResults.size() >= maxResults) {
                        break;
                    }
                }
            }
        }

        // earlier datetimes go last
        qSort(results.begin(), results.end(),
              [](const QPair<QDateTime, QString> &a1, const QPair<QDateTime, QString> &a2) -> bool {
            return a1.first > a2.first;
        });

        foreach (auto &pair, results) {
            auto &filepath = pair.second;
            Q_ASSERT(m_LocalArtworks.contains(filepath));
            auto &localData = m_LocalArtworks[filepath];
            searchResults.emplace_back(new SuggestionArtwork(filepath, localData.m_Title, localData.m_Description, localData.m_Keywords));
        }
    }

    void LocalLibrary::cleanupTrash() {
        QMutexLocker locker(&m_Mutex);

        QStringList itemsToRemove;
        QHashIterator<QString, LocalArtworkData> i(m_LocalArtworks);

        while (i.hasNext()) {
            i.next();

            QFile file(i.key());
            if (!file.exists()) {
                itemsToRemove.append(i.key());
            }
        }

        foreach (const QString &item, itemsToRemove) {
            m_LocalArtworks.remove(item);
        }

        LOG_INFO << itemsToRemove.count() << "item(s) removed.";
    }

    void LocalLibrary::artworksAdded() {
        saveLibraryAsync();
    }

    void LocalLibrary::saveLibraryAsync() {
        performAsync(LibraryLoaderWorker::Save);
    }

    void LocalLibrary::performAsync(LibraryLoaderWorker::LoadOption option) {
        LOG_DEBUG << option;
        LibraryLoaderWorker *worker = new LibraryLoaderWorker(this, m_Filename, option);
        QThread *thread = new QThread();
        worker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(worker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(worker, SIGNAL(stopped()), worker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }

    void LocalLibrary::doAddToLibrary(const QVector<Models::ArtworkMetadata *> artworksList) {
        int length = artworksList.length();

        LOG_DEBUG << length << "file(s)";

        QMutexLocker locker(&m_Mutex);

        for (int i = 0; i < length; ++i) {
            Models::ArtworkMetadata *metadata = artworksList.at(i);
            const QString &filepath = metadata->getFilepath();
            QFileInfo fi(filepath);

            LocalArtworkData data;
            data.m_ArtworkType = LocalArtworkImage;
            data.m_Title = metadata->getTitle();
            data.m_Description = metadata->getDescription();
            data.m_Keywords = metadata->getKeywords();
            Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork *>(metadata);
            if (image != nullptr) {
                if (image->hasVectorAttached()) {
                    data.m_ArtworkType = LocalArtworkVector;
                }
            }

            if (fi.exists()) {
                data.m_CreationTime = fi.created();
            }

            // replaces if exists
            m_LocalArtworks.insert(filepath, data);
        }

        LOG_INFO << length << "item(s) updated or added";
    }

    void LocalLibrary::cleanupLocalLibraryAsync() {
        performAsync(LibraryLoaderWorker::Clean);
    }
}
