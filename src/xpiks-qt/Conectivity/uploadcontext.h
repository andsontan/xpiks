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

#ifndef UPLOADCONTEXT
#define UPLOADCONTEXT

#include <QString>
#include "../Common/defines.h"

namespace Models {
    class ProxySettings;
}

namespace Conectivity {
    class UploadContext {
    public:
        ~UploadContext() {
            LOG_DEBUG << "destructor for host" << m_Host;
        }

    public:
        QString m_Host;
        QString m_Username;
        QString m_Password;
        QString m_DirForVectors;
        QString m_DirForImages;
        bool m_UsePassiveMode;
        bool m_UseEPSV;
        int m_RetriesCount;
        int m_TimeoutSeconds;
        bool m_UseProxy;
        bool m_VerboseLogging;
        Models::ProxySettings *m_ProxySettings;
    };
}

#endif // UPLOADCONTEXT

