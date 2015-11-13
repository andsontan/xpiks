/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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


#ifndef UPLOADITEM
#define UPLOADITEM

#include <QString>
#include "../Models/uploadinfo.h"

namespace Helpers {
    class UploadItem {
    public:
        UploadItem () {
        }

        UploadItem(Models::UploadInfo *uploadInfo, const QStringList &filesToUpload,
                   int uploadTimeout,
                   const QString &curlPath, const QString &proxyURI) :
            m_UploadInfo (uploadInfo),
            m_FilesToUpload(filesToUpload),
            m_OneItemUploadMinutesTimeout(uploadTimeout),
            m_CurlPath(curlPath),
            m_ProxyURI(proxyURI),
            m_Success(false)
        { }

        UploadItem(const UploadItem &copy) :
            m_UploadInfo(copy.m_UploadInfo),
            m_FilesToUpload(copy.m_FilesToUpload),
            m_OneItemUploadMinutesTimeout(copy.m_OneItemUploadMinutesTimeout),
            m_CurlPath(copy.m_CurlPath),
            m_ProxyURI(copy.m_ProxyURI),
            m_Success(copy.m_Success)
        { }

    public:
        Models::UploadInfo *m_UploadInfo;
        QStringList m_FilesToUpload;
        int m_OneItemUploadMinutesTimeout;
        QString m_CurlPath;
        QString m_ProxyURI;
        bool m_Success;
    };
}

#endif // UPLOADITEM

