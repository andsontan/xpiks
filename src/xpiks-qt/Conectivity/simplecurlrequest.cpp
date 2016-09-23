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

#include "simplecurlrequest.h"
#include <curl/curl.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include "../Common/defines.h"

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

namespace Conectivity {
    SimpleCurlRequest::SimpleCurlRequest(const QString &resource, bool verifySSL, QObject *parent) :
        QObject(parent),
        m_RemoteResource(resource),
        m_VerifySSL(verifySSL)
    {
    }

    bool SimpleCurlRequest::sendRequestSync() {
        return doRequest();
    }

    void SimpleCurlRequest::setRawHeaders(const QStringList &headers) {
        m_RawHeaders = headers;
    }

    void SimpleCurlRequest::process() {
        bool success = doRequest();
        emit requestFinished(success);
    }

    bool SimpleCurlRequest::doRequest() {
        // https://curl.haxx.se/libcurl/c/getinmemory.html
        CURL *curl_handle = nullptr;
        CURLcode res;

        MemoryStruct chunk;
        chunk.memory = nullptr;
        chunk.size = 0;

        struct curl_slist *curl_headers = NULL;

        /* init the curl session */
        curl_handle = curl_easy_init();

        if (!curl_handle) {
            return false;
        }

        std::string resourceString = m_RemoteResource.toStdString();
        const char *url = resourceString.data();
        /* specify URL to get */
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);

        if (!m_VerifySSL) {
            /*
             * If you want to connect to a site who isn't using a certificate that is
             * signed by one of the certs in the CA bundle you have, you can skip the
             * verification of the server's certificate. This makes the connection
             * A LOT LESS SECURE.
             *
             * If you have a CA cert for the server stored someplace else than in the
             * default bundle, then the CURLOPT_CAPATH option might come handy for
             * you.
            */
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

            /*
             * If the site you're connecting to uses a different host name that what
             * they have mentioned in their server certificate's commonName (or
             * subjectAltName) fields, libcurl will refuse to connect. You can skip
             * this check, but this will make the connection less secure.
            */
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        /* some servers don't like requests that are made without a user-agent
             field, so we provide one */
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        if (!m_RawHeaders.empty()) {
            foreach (const QString &header, m_RawHeaders) {
                std::string headerString = header.toStdString();
                curl_headers = curl_slist_append(curl_headers, headerString.data());
            }

            /* set our custom set of headers */
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, curl_headers);
        }

        /* get it! */
        res = curl_easy_perform(curl_handle);

        const bool success = (CURLE_OK == res);

        /* check for errors */
        if (!success) {
            m_ErrorString = QString::fromLatin1(curl_easy_strerror(res));
            LOG_WARNING << "curl_easy_perform() failed" << m_ErrorString;
        } else {
            /*
             * Now, our chunk.memory points to a memory block that is chunk.size
             * bytes big and contains the remote file.
             *
             * Do something nice with it!
             */

            LOG_INFO << chunk.size << "bytes received";
            m_ResponseData = QByteArray(chunk.memory, (uint)chunk.size);
        }

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        /* free the custom headers */
        if (curl_headers != NULL) {
            curl_slist_free_all(curl_headers);
        }

        free(chunk.memory);

        return success;
    }
}
