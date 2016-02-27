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

#include "artworksprocessor.h"
#include <QtConcurrent>

namespace Models {
    void ArtworksProcessor::resetModel() {
        qDebug() << "ArtworksProcessor::resetModel #";
        setInProgress(false);
        setIsError(false);
        innerResetModel();
        m_ArtworksCount = 0;
        m_ProcessedArtworksCount = 0;
        updateProgress();
    }

    void ArtworksProcessor::beginProcessing() {
        m_ExistingMaxThreadsNumber = QThreadPool::globalInstance()->maxThreadCount();
        qDebug() << "ArtworksProcessor::beginProcessing #" << "Saving pools max threads" << m_ExistingMaxThreadsNumber;
        m_ArtworksCount = m_ArtworkList.length();
        m_ProcessedArtworksCount = 0;
        updateProgress();
        setInProgress(true);
        emit startedProcessing();
    }

    void ArtworksProcessor::endProcessing() {
        m_ProcessedArtworksCount = 0;
        m_ArtworksCount = 0;
        qDebug() << "ArtworksProcessor::endProcessing #" << "Restoring pool max threads to" << m_ExistingMaxThreadsNumber;
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
        qDebug() << "ArtworksProcessor::restrictMaxThreads #" << (int)MAX_WORKER_THREADS;
        QThreadPool::globalInstance()->setMaxThreadCount((int)MAX_WORKER_THREADS);
    }
}
