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

#include "testconnection.h"
#include "uploadcontext.h"
#include "ftphelpers.h"
#include <curl/curl.h>
#include "../Common/defines.h"
#include "../Models/proxysettings.h"
#include "../Helpers/stringhelper.h"

namespace Conectivity {
    static size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data) {
        (void)ptr;
        (void)data;
        /* we are not interested in the headers itself,
         so we only return the size we would have saved ... */
        return (size_t)(size * nmemb);
    }

    ContextValidationResult isContextValid(Conectivity::UploadContext *context) {
        bool result = false;
        CURL *curlHandle = NULL;
        QString host = sanitizeHost(context->m_Host);
        const QString &remoteUrl = host;

        // should be called in main() using helper
        //curl_global_init(CURL_GLOBAL_ALL);
        curlHandle = curl_easy_init();

        CURLcode r = CURLE_GOT_NOTHING;

        fillCurlOptions(curlHandle, context, remoteUrl);

        curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_DIRLISTONLY, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, throw_away);
        curl_easy_setopt(curlHandle, CURLOPT_FTP_USE_EPSV, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);

        LOG_DEBUG << "About to check credentials for" << host;
        r = curl_easy_perform(curlHandle);

        result = (CURLE_OK == r);

        if (!result) {
            LOG_INFO << "Credentials checking error:" << curl_easy_strerror(r);
        }

        curl_easy_cleanup(curlHandle);
        // should be called in main()
        //curl_global_cleanup();

        LOG_INFO << "Credentials checking finished" << host;
        delete context;

        ContextValidationResult cvr;
        cvr.m_Host = host;
        cvr.m_Result = result;

        return cvr;
    }
}
