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

#include "ftphelpers.h"
#include "uploadcontext.h"
#include <cstdio>
#include <cstdlib>
#include <curl/curl.h>
#include "../Models/proxysettings.h"

namespace Conectivity {
    /* The MinGW headers are missing a few Win32 function definitions,
       you shouldn't need this if you use VC++ */
#if defined(__MINGW32__) && !defined(__MINGW64__)
    int __cdecl _snscanf(const char *input, size_t length, const char *format, ...);

#endif

    /* parse headers for Content-Length */
    size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
        int r;
        long len = 0;

#if defined(Q_OS_WIN)
        /* _snscanf() is Win32 specific */
        r = _snscanf((const char *)ptr, size * nmemb, "Content-Length: %ld\n", &len);
#else
        r = std::sscanf((const char *)ptr, "Content-Length: %ld\n", &len);
#endif

        if (r) { /* Microsoft: we don't read the specs */
            *((long *)stream) = len;
        }

        return size * nmemb;
    }

    /* discard downloaded data */
    size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
        Q_UNUSED(stream);
        Q_UNUSED(ptr);
        return size * nmemb;
    }

    /* read data to upload */
    size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
        FILE *f = (FILE *)stream;
        size_t n;

        if (ferror(f)) {
            return CURL_READFUNC_ABORT;
        }

        n = fread(ptr, size, nmemb, f) * size;

        return n;
    }

    void fillCurlOptions(void *curlHandle, UploadContext *context, const QString &remoteUrl) {
        curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curlHandle, CURLOPT_URL, remoteUrl.toLocal8Bit().data());
        curl_easy_setopt(curlHandle, CURLOPT_USERNAME, context->m_Username.toLocal8Bit().data());
        curl_easy_setopt(curlHandle, CURLOPT_PASSWORD, context->m_Password.toLocal8Bit().data());

        if (context->m_TimeoutSeconds) {
            curl_easy_setopt(curlHandle, CURLOPT_FTP_RESPONSE_TIMEOUT, context->m_TimeoutSeconds);
        }

        curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);

        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, discardfunc);

        curl_easy_setopt(curlHandle, CURLOPT_READFUNCTION, readfunc);

        if (!context->m_UsePassiveMode) {
            curl_easy_setopt(curlHandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */
        }

        if (context->m_UseProxy) {
            Models::ProxySettings *proxySettings = context->m_ProxySettings;
            Q_ASSERT(proxySettings != NULL);

            curl_easy_setopt(curlHandle, CURLOPT_PROXY, proxySettings->m_Address.toLocal8Bit().data());

            const QString &proxyUser = proxySettings->m_User;
            if (!proxyUser.isEmpty()) {
                curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERNAME, proxyUser.toLocal8Bit().data());

                const QString &proxyPassword = proxySettings->m_Password;
                if (!proxyPassword.isEmpty()) {
                    curl_easy_setopt(curlHandle, CURLOPT_PROXYPASSWORD, proxyPassword.toLocal8Bit().data());
                }
            }

            curl_easy_setopt(curlHandle, CURLOPT_PROXYPORT, proxySettings->m_Port);
        }
    }

    QString sanitizeHost(const QString &inputHost) {
        QString host = inputHost;

        const QChar slash('/');

        if (!host.endsWith(slash)) {
            host.append(slash);
        }

        if (!host.startsWith(QLatin1String("ftp.")) &&
            !host.startsWith(QLatin1String("ftp://"))) {
            host = QLatin1String("ftp://") + host;
        }

        return host;
    }
}
