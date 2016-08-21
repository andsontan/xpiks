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

#include "imagecachingworker.h"
#include <QDir>
#include <QFile>
#include <QImage>
#include <QString>
#include <QFileInfo>
#include <QByteArray>
#include <QDataStream>
#include <QReadLocker>
#include <QWriteLocker>
#include <QCryptographicHash>
#include "../Common/defines.h"
#include "../Helpers/constants.h"
#include "imagecacherequest.h"

namespace QMLExtensions {
    QString getPathHash(const QString &path) {
        return QString::fromLatin1(QCryptographicHash::hash(path.toUtf8(), QCryptographicHash::Sha256).toHex());
    }

    QDataStream &operator<<(QDataStream &out, const CachedImage &v) {
        out << v.m_Filename << v.m_LastModified << v.m_Size << v.m_RequestsServed << v.m_AdditionalData;
        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedImage &v) {
        in >> v.m_Filename >> v.m_LastModified >> v.m_Size >> v.m_RequestsServed >> v.m_AdditionalData;
        return in;
    }

    ImageCachingWorker::ImageCachingWorker(QObject *parent):
        QObject(parent),
        m_ProcessedItemsCount(0)
    {
    }

    bool ImageCachingWorker::initWorker() {
        LOG_DEBUG << "#";

        m_ProcessedItemsCount = 0;
        QString appDataPath = XPIKS_USERDATA_PATH;

        if (!appDataPath.isEmpty()) {
            m_ImagesCacheDir = QDir::cleanPath(appDataPath + QDir::separator() + Constants::IMAGES_CACHE_DIR);
            QDir appDataDir(appDataPath);
            m_IndexFilepath = appDataDir.filePath(Constants::IMAGES_CACHE_INDEX);

            QDir imagesCacheDir(m_ImagesCacheDir);
            if (!imagesCacheDir.exists()) {
                LOG_INFO << "Creating cache dir" << m_ImagesCacheDir;
                QDir().mkpath(m_ImagesCacheDir);
            }
        } else {
            m_ImagesCacheDir = QDir::currentPath();
            m_IndexFilepath = Constants::IMAGES_CACHE_INDEX;
        }

        LOG_INFO << "Using" << m_ImagesCacheDir << "for images cache";

        readIndex();

        return true;
    }

    void ImageCachingWorker::processOneItem(std::shared_ptr<ImageCacheRequest> &item) {
        if (isProcessed(item)) { return; }

        const QString &originalPath = item->getFilepath();
        QSize requestedSize = item->getRequestedSize();

        LOG_INFO << (item->getNeedRecache() ? "Recaching" : "Caching") << originalPath << "with size" << requestedSize;

        if (!requestedSize.isValid()) {
            LOG_WARNING << "Invalid requestedSize for" << originalPath;
            requestedSize.setHeight(150);
            requestedSize.setWidth(150);
        }

        QImage img(originalPath);
        QImage resizedImage = img.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QFileInfo fi(originalPath);
        QString pathHash = getPathHash(originalPath) + "." + fi.completeSuffix();
        QString cachedFilepath = QDir::cleanPath(m_ImagesCacheDir + QDir::separator() + pathHash);

        if (resizedImage.save(cachedFilepath)) {
            CachedImage cachedImage;
            cachedImage.m_Filename = pathHash;
            cachedImage.m_LastModified = fi.lastModified();
            cachedImage.m_Size = requestedSize;

            {
                QWriteLocker locker(&m_CacheLock);
                Q_UNUSED(locker);

                if (m_CacheIndex.contains(originalPath)) {
                    cachedImage.m_RequestsServed = m_CacheIndex[originalPath].m_RequestsServed + 1;
                } else {
                    cachedImage.m_RequestsServed = 1;
                }

                m_CacheIndex.insert(originalPath, cachedImage);
            }

            m_ProcessedItemsCount++;
        } else {
            LOG_WARNING << "Failed to save image. Path:" << cachedFilepath << "size" << requestedSize;
        }

        if (m_ProcessedItemsCount % 50 == 0) {
            saveIndex();
        }

        if (item->getWithDelay()) {
            // force context switch for more imporant tasks
            QThread::msleep(500);
        }
    }

    bool ImageCachingWorker::tryGetCachedImage(const QString &key, const QSize &requestedSize,
                                               QString &cachedPath, bool &needsUpdate) {
        bool found = false;
        QReadLocker locker(&m_CacheLock);
        Q_UNUSED(locker);

        if (m_CacheIndex.contains(key)) {
            CachedImage &cachedImage = m_CacheIndex[key];
            QString cachedValue = QDir::cleanPath(m_ImagesCacheDir + QDir::separator() + cachedImage.m_Filename);

            QFileInfo fi(cachedValue);

            if (fi.exists()) {
                cachedImage.m_RequestsServed++;
                cachedPath = cachedValue;
                needsUpdate = (QFileInfo(key).lastModified() > cachedImage.m_LastModified) || (cachedImage.m_Size != requestedSize);

                found = true;
            }
        }

        return found;
    }

    void ImageCachingWorker::splitToCachedAndNot(const std::vector<std::shared_ptr<ImageCacheRequest> > allRequests,
                                                 std::vector<std::shared_ptr<ImageCacheRequest> > &unknownRequests,
                                                 std::vector<std::shared_ptr<ImageCacheRequest> > &knownRequests) {
        size_t size = allRequests.size();
        if (size == 0) { return; }

        LOG_DEBUG << "#";

        QReadLocker locker(&m_CacheLock);
        Q_UNUSED(locker);

        knownRequests.reserve(size);
        unknownRequests.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            auto &item = allRequests.at(i);

            if (m_CacheIndex.contains(item->getFilepath())) {
                knownRequests.push_back(item);
            } else {
                unknownRequests.push_back(item);
            }
        }
    }

    void ImageCachingWorker::readIndex() {
        QFile file(m_IndexFilepath);
        if (file.open(QIODevice::ReadOnly)) {
            QHash<QString, CachedImage> cacheIndex;

            QDataStream in(&file);   // read the data
            in >> cacheIndex;
            file.close();

            m_CacheIndex.swap(cacheIndex);
            LOG_INFO << "Images cache index read:" << m_CacheIndex.size() << "entries";
        } else {
            LOG_WARNING << "File not found:" << m_IndexFilepath;
        }
    }

    void ImageCachingWorker::saveIndex() {
        LOG_DEBUG << "#";

        QFile file(m_IndexFilepath);

        if (file.open(QIODevice::WriteOnly)) {
            QReadLocker locker(&m_CacheLock);
            Q_UNUSED(locker);

            QDataStream out(&file);   // write the data
            out << m_CacheIndex;
            file.close();
            LOG_INFO << "Images cache index saved:" << m_CacheIndex.size() << "entries";
        }
    }

    bool ImageCachingWorker::isProcessed(std::shared_ptr<ImageCacheRequest> &item) {
        if (item->getNeedRecache()) { return false; }

        const QString &originalPath = item->getFilepath();
        const QSize &requestedSize = item->getRequestedSize();

        bool isAlreadyProcessed = false;

        QString cachedPath;
        bool needsUpdate = false;
        if (this->tryGetCachedImage(originalPath, requestedSize, cachedPath, needsUpdate)) {
            isAlreadyProcessed = !needsUpdate;
        }

        return isAlreadyProcessed;
    }
}
