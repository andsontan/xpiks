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
#include <sstream>
#include <string>
#include <curl/curl.h>
#include "../Models/proxysettings.h"
#include "../Helpers/stringhelper.h"

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

    static
    QString sanitizeCurlLogline(const std::string &str) {
        QString logline = QString::fromStdString(str).trimmed();
#ifndef QT_DEBUG
        if (!logline.contains("PASS "))
#endif
        {
            return logline;
        }
#ifndef QT_DEBUG
        else {
            return QLatin1String("Password token hidden here");
        }
#endif
    }

    static
    void dump(const char *text,
              unsigned char *ptr, size_t size) {
        std::stringstream ss;
        size_t i;
        size_t c;
        unsigned int width = 0x10;

        ss << Helpers::string_format("%s, %10.10ld bytes (0x%8.8lx)\n",
                text, (long)size, (long)size);

        if (size > 256) {
            return;
        }

        for (i = 0; i < size; i += width) {
            //ss << string_format("%4.4lx: ", (long)i);

            /* show hex to the left
            for (c = 0; c < width; c++) {
                if (i + c < size)
                    ss << string_format("%02x ", ptr[i+c]);
                else
                    ss << "   ";
            }*/

            /* show data on the right */
            for(c = 0; (c < width) && (i+c < size); c++) {
                char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
                ss << x;
            }

            //ss << "\n";
        }

        LOG_DEBUG << sanitizeCurlLogline(ss.str());
    }

    static
    int my_trace(CURL *handle, curl_infotype type,
                 char *data, size_t size,
                 void *userp) {
      const char *text;
      (void)handle; /* prevent compiler warning */
      (void)userp;

      switch (type) {
      case CURLINFO_TEXT:
          LOG_INFO << sanitizeCurlLogline(Helpers::string_format("== Info: %s", data));
      default: /* in case a new one is introduced to shock us */
          return 0;

      case CURLINFO_HEADER_OUT:
          text = "=> Send header";
          break;
      case CURLINFO_DATA_OUT:
          text = "=> Send data";
          break;
      case CURLINFO_SSL_DATA_OUT:
          text = "=> Send SSL data";
          break;
      case CURLINFO_HEADER_IN:
          text = "<= Recv header";
          break;
      case CURLINFO_DATA_IN:
          text = "<= Recv data";
          break;
      case CURLINFO_SSL_DATA_IN:
          text = "<= Recv SSL data";
          break;
      }

      dump(text, (unsigned char *)data, size);
      return 0;
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

        curl_easy_setopt(curlHandle, CURLOPT_DEBUGFUNCTION, my_trace);

        if (!context->m_UsePassiveMode) {
            LOG_DEBUG << "Passive mode disabled";
            curl_easy_setopt(curlHandle, CURLOPT_FTPPORT, "-"); /* disable passive mode */
        }

        if (!context->m_UseEPSV) {
            LOG_DEBUG << "EPSV mode disabled";
            curl_easy_setopt(curlHandle, CURLOPT_FTP_USE_EPSV, 0L);
        }

        fillProxySettings(curlHandle, context);
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

    void fillProxySettings(void *curlHandle, UploadContext *context) {
        if (context->m_UseProxy) {
            Models::ProxySettings *proxySettings = context->m_ProxySettings;
            Q_ASSERT(proxySettings != NULL);

            curl_easy_setopt(curlHandle, CURLOPT_PROXY, proxySettings->m_Address.toLocal8Bit().data());
            LOG_DEBUG << "Using proxy:" << proxySettings->m_Address;

            const QString &proxyUser = proxySettings->m_User;
            if (!proxyUser.isEmpty()) {
                curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERNAME, proxyUser.toLocal8Bit().data());

                const QString &proxyPassword = proxySettings->m_Password;
                if (!proxyPassword.isEmpty()) {
                    curl_easy_setopt(curlHandle, CURLOPT_PROXYPASSWORD, proxyPassword.toLocal8Bit().data());
                }
            }

            if (!proxySettings->m_Port.isEmpty()) {
                bool isOk = false;
                int port = proxySettings->m_Port.toInt(&isOk);
                if (isOk && (port != 0)) {
                    LOG_INFO << "Using proxy port:" << port;
                    curl_easy_setopt(curlHandle, CURLOPT_PROXYPORT, port);
                } else {
                    LOG_INFO << "Failed to parse port:" << proxySettings->m_Port;
                }
            }
        }
    }
}
