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

#include "simplecurldownloader.h"
#include <QFile>
#include <QTemporaryFile>
#include <cstdio>
#include <curl/curl.h>
#include "../Common/defines.h"
#include "../Models/proxysettings.h"
#include "ftphelpers.h"

static size_t write_file(void *buffer, size_t size, size_t nmemb, void *param) {
    QFile *out = (QFile *)param;

    char *bytes = (char *)buffer;
    qint64 length = 0;
    length = nmemb * size;

    qint64 written = out->write(bytes, length);
    return (size_t)written;
}

namespace Conectivity {
    SimpleCurlDownloader::SimpleCurlDownloader(const QString &resource, QObject *parent) :
        QObject(parent),
        m_RemoteResource(resource),
        m_ProxySettings(nullptr)
    {
        m_TempFile.setAutoRemove(false);
    }

    SimpleCurlDownloader::~SimpleCurlDownloader() {
        m_TempFile.remove();
    }

    bool SimpleCurlDownloader::downloadFileSync() {
        return doDownloadFile();
    }

    void SimpleCurlDownloader::setProxySettings(Models::ProxySettings *proxySettings) {
        m_ProxySettings = proxySettings;
    }

    void SimpleCurlDownloader::process() {
        bool success = doDownloadFile();
        emit downloadFinished(success);
    }

    bool SimpleCurlDownloader::doDownloadFile() {
        if (!m_TempFile.open()) {
            return false;
        }

        m_TempFile.setAutoRemove(false);

        CURL *curl_handle = nullptr;
        CURLcode res;

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
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_file);

        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&m_TempFile);

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

        if (m_ProxySettings != nullptr) {
            fillProxySettings(curl_handle, m_ProxySettings);
        }

        /* get it! */
        res = curl_easy_perform(curl_handle);

        const bool success = (CURLE_OK == res);

        /* check for errors */
        if (!success) {
            m_ErrorString = QString::fromLatin1(curl_easy_strerror(res));
            LOG_WARNING << "curl_easy_perform() failed" << m_ErrorString;
        }

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        return success;
    }
}
