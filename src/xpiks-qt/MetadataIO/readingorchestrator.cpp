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

#include "readingorchestrator.h"
#include <QThread>
#include <QVector>
#include "../xpiks-qt/Models/artworkmetadata.h"

#define MIN_SPLIT_COUNT 10
#define MAX_READING_THREADS 10
#define MIN_READING_THREADS 1

namespace MetadataIO {
    ReadingOrchestrator::ReadingOrchestrator(const QVector<Models::ArtworkMetadata *> &itemsToRead,
                                             QObject *parent) :
        QObject(parent),
        m_ThreadsCount(MIN_READING_THREADS),
        m_FinishedCount(0)
    {
        int size = itemsToRead.size();
        if (size > MIN_SPLIT_COUNT) {
            m_ThreadsCount = qMin(qMax(QThread::idealThreadCount(), MIN_READING_THREADS), MAX_READING_THREADS);
            int chunkSize = size / m_ThreadsCount;

            int left = 0;
            while (left < size) {
                m_ItemsToRead.push_back(itemsToRead.mid(left, chunkSize));
                left += chunkSize;
            }
        } else {
            m_ItemsToRead.push_back(itemsToRead);
        }
    }
}
