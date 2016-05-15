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

#include "exiv2writingworker.h"
#include "../Models/artworkmetadata.h"
#include "../Common/defines.h"
#include <exiv2/exiv2.hpp>

namespace MetadataIO {


    Exiv2WritingWorker::Exiv2WritingWorker(int index, QVector<Models::ArtworkMetadata *> itemsToWrite, QObject *parent) :
        QObject(parent),
        m_ItemsToWrite(itemsToWrite),
        m_WorkerIndex(index),
        m_Stopped(false)
    {
        Q_ASSERT(!itemsToWrite.isEmpty());
        LOG_INFO << "Worker [" << index << "]:" << itemsToWrite.size() << "items to read";
    }

    void Exiv2WritingWorker::process() {
        bool anyError = false;
        int size = m_ItemsToWrite.size();

        for (int i = 0; i < size; ++i) {
            if (m_Stopped) { break; }

            Models::ArtworkMetadata *artwork = m_ItemsToWrite.at(i);

            try {
                writeMetadata(artwork);
            }
            catch(Exiv2::Error &error) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Exiv2 error:" << error.what();
            }
            catch(...) {
                anyError = true;
                LOG_WARNING << "Worker" << m_WorkerIndex << "Writing error for item" << artwork->getFilepath();
            }
        }

        LOG_INFO << "Worker #" << m_WorkerIndex << "finished";

        emit finished(anyError);
    }

    void Exiv2WritingWorker::cancel() {
        m_Stopped = true;
    }

    void Exiv2WritingWorker::writeMetadata(Models::ArtworkMetadata *artwork) {

    }
}
