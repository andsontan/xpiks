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

#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QString>

#if defined(Q_OS_DARWIN)
#define UPDATE_JSON_URL_SUFFIX "update-osx.json"
#elif defined(Q_OS_WIN64)
#define UPDATE_JSON_URL_SUFFIX "update-windows.json"
#elif defined(Q_OS_WIN32)
#define UPDATE_JSON_URL_SUFFIX "update-windows-32.json"
#else
#define UPDATE_JSON_URL_SUFFIX "update.json"
#endif

namespace Conectivity {
    class ApiManager
    {
    public:
        static ApiManager& getInstance()
        {
            static ApiManager instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    public:
        QString getUpdateAddr() const {
            return m_BaseUrl + QLatin1String(UPDATE_JSON_URL_SUFFIX);
        }

        QString getDefaultUpdateAddr() const {
            return m_BaseUrl + "update.json";
        }

        QString getStocksACSourceAddr() const {
            return m_BaseUrl + "stocks_ftp.json";
        }

        QString getWarningSettingsAddr() const {
            return m_BaseUrl + "warning_settings.json";
        }

    private:
        ApiManager() {
#ifdef QT_DEBUG
            m_BaseUrl = "https://ribtoks.github.io/xpiks/api/dev/";
#else
            m_BaseUrl = "https://ribtoks.github.io/xpiks/api/v1/";
#endif
        }

        ApiManager(ApiManager const&);
        void operator=(ApiManager const&);

    private:
        QString m_BaseUrl;
    };
}

#endif // APIMANAGER_H
