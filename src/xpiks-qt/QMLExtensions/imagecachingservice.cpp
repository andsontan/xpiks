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

#include "imagecachingservice.h"
#include <QThread>
#include "imagecachingworker.h"
#include "imagecacherequest.h"
#include "../Models/artworkmetadata.h"

#define DEFAULT_THUMB_HEIGHT 300
#define DEFAULT_THUMB_WIDTH 300

namespace QMLExtensions {
    ImageCachingService::ImageCachingService(QObject *parent) :
        QObject(parent),
        m_CachingWorker(NULL)
    {
    }

    void ImageCachingService::startService() {
        m_CachingWorker = new ImageCachingWorker();

        QThread *thread = new QThread();
        m_CachingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_CachingWorker, SLOT(process()));
        QObject::connect(m_CachingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_CachingWorker, SIGNAL(stopped()), m_CachingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        LOG_DEBUG << "starting thread...";
        thread->start();
    }

    void ImageCachingService::stopService() {
        LOG_DEBUG << "#";

        if (m_CachingWorker != NULL) {
            m_CachingWorker->stopWorking();
        } else {
            LOG_WARNING << "Caching Worker was NULL";
        }
    }

    void ImageCachingService::cacheImage(const QString &key, const QSize &requestedSize, bool recache) {
        Q_ASSERT(m_CachingWorker != NULL);
        ImageCacheRequest *request = new ImageCacheRequest(key, requestedSize, recache);
        m_CachingWorker->submitItem(request);
    }

    void ImageCachingService::generatePreviews(const QVector<Models::ArtworkMetadata *> &items) {
        Q_ASSERT(m_CachingWorker != NULL);
        LOG_INFO << "generating for" << items.size() << "items";
        QVector<ImageCacheRequest *> requests;
        int size = items.size();
        requests.reserve(size);
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *artwork = items.at(i);
            ImageCacheRequest *request = new ImageCacheRequest(artwork->getFilepath(),
                                                               QSize(DEFAULT_THUMB_WIDTH, DEFAULT_THUMB_HEIGHT),
                                                               false);
            requests.append(request);
        }

        m_CachingWorker->submitItems(requests);
    }

    bool ImageCachingService::tryGetCachedImage(const QString &key, const QSize &requestedSize,
                                                QString &cached, bool &needsUpdate) {
        if (m_CachingWorker != NULL) {
            return m_CachingWorker->tryGetCachedImage(key, requestedSize, cached, needsUpdate);
        } else {
            return false;
        }
    }
}
