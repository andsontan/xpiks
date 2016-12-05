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

#include "cachingimageprovider.h"
#include "../Common/defines.h"
#include "../QMLExtensions/imagecachingservice.h"

#define RECACHE true

namespace QMLExtensions {
    QImage CachingImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
        QString cachedPath;
        bool needsUpdate = false;

        if (m_ImageCachingService->tryGetCachedImage(id, requestedSize, cachedPath, needsUpdate)) {
            QImage image(cachedPath);
            *size = image.size();

            if (needsUpdate) {
                LOG_INFO << "Recaching image" << id;
                m_ImageCachingService->cacheImage(id, requestedSize, RECACHE);
            }

            return image;
        } else {
            LOG_INTEGR_TESTS_OR_DEBUG << "Not found cached:" << id;

            QImage image(id);
            QImage result;

            if (requestedSize.isValid()) {
                m_ImageCachingService->cacheImage(id, requestedSize);
                result = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                LOG_WARNING << "Size is invalid:" << requestedSize.width() << "x" << requestedSize.height();
                result = image;
            }

            *size = result.size();
            return result;
        }
    }
}
