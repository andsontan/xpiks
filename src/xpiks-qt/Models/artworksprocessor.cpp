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

#include "artworksprocessor.h"
#include <QtConcurrent>
#include "../Common/defines.h"

namespace Models {
    void ArtworksProcessor::resetModel() {
        LOG_DEBUG << "#";
        setInProgress(false);
        setIsError(false);
        innerResetModel();
        m_ArtworksCount = 0;
        m_ProcessedArtworksCount = 0;
        updateProgress();
    }

    bool ArtworksProcessor::removeUnavailableItems() {
        LOG_DEBUG << "#";

        const QVector<Models::ArtworkMetadata*> &artworksListOld = getArtworkList();
        QVector<Models::ArtworkMetadata*> artworksListNew;

        int size = artworksListOld.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata* item = artworksListOld.at(i);

            if (!item->isUnavailable()) {
                artworksListNew.append(item);
            }
        }

        bool anyUnavailable = artworksListNew.size() != m_ArtworkList.size();

        setArtworks(artworksListNew);

        if (artworksListNew.isEmpty()) {
            emit requestCloseWindow();
        }

        emit itemsNumberChanged();

        return anyUnavailable;
    }

    void ArtworksProcessor::beginProcessing() {
        m_ExistingMaxThreadsNumber = QThreadPool::globalInstance()->maxThreadCount();
        LOG_DEBUG << "Saving pools max threads" << m_ExistingMaxThreadsNumber;
        m_ArtworksCount = m_ArtworkList.length();
        m_ProcessedArtworksCount = 0;
        updateProgress();
        setInProgress(true);
        emit startedProcessing();
    }

    void ArtworksProcessor::endProcessing() {
        m_ProcessedArtworksCount = 0;
        m_ArtworksCount = 0;
        LOG_DEBUG << "Restoring pool max threads to" << m_ExistingMaxThreadsNumber;
        QThreadPool::globalInstance()->setMaxThreadCount(m_ExistingMaxThreadsNumber);
        setInProgress(false);
        emit finishedProcessing();
    }

    void ArtworksProcessor::endAfterFirstError() {
        setIsError(true);
        m_ProcessedArtworksCount = m_ArtworksCount;
        updateProgress();
        endProcessing();
    }

    void ArtworksProcessor::restrictMaxThreads() {
        LOG_DEBUG << (int)MAX_WORKER_THREADS;
        QThreadPool::globalInstance()->setMaxThreadCount((int)MAX_WORKER_THREADS);
    }
}
