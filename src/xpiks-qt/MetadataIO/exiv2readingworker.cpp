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

#include "exiv2readingworker.h"
#include <QVector>
#include "../xpiks-qt/Models/artworkmetadata.h"
#include "../xpiks-qt/Common/defines.h"
#include <exiv2/exiv2.hpp>

namespace MetadataIO {
    Exiv2ReadingWorker::Exiv2ReadingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToRead, QObject *parent):
        QObject(parent),
        m_ItemsToRead(itemsToRead),
        m_WorkerIndex(index),
        m_Stopped(false)
    {
        Q_ASSERT(!itemsToRead.isEmpty());
        LOG_INFO << itemsToRead.size() << "items to read";
    }

    Exiv2ReadingWorker::~Exiv2ReadingWorker() {
        LOG_DEBUG << "Reading worker" << m_WorkerIndex << "destroyed";
    }

    void Exiv2ReadingWorker::process() {
        LOG_DEBUG << "Worker #" << m_WorkerIndex << "started";

        bool anyError = false;

        int size = m_ItemsToRead.size();
        for (int i = 0; i < size; ++i) {
            if (m_Stopped) { break; }

            Models::ArtworkMetadata *artwork = m_ItemsToRead.at(i);
            const QString &filepath = artwork->getFilepath();
            ImportDataResult importResult;

            try {
                if (readMetadata(artwork, importResult)) {
                    Q_ASSERT(!m_ImportResult.contains(filepath));
                    m_ImportResult.insert(filepath, importResult);
                }
            }
            catch(Exiv2::Error &error) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Exiv2 error:" << error.what();
            }
            catch(...) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Reading error for item" << filepath;
            }
        }

        LOG_INFO << "Worker #" << m_WorkerIndex << "finished";

        emit finished(anyError);
    }

    void Exiv2ReadingWorker::cancel() {
        m_Stopped = true;
    }

    bool Exiv2ReadingWorker::readMetadata(Models::ArtworkMetadata *artwork, ImportDataResult &importResult) {
        // BUMP
    }
}
