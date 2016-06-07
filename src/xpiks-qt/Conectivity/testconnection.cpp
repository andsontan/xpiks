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
#include <sstream>
#include <stdarg.h>
#include <string>
#include "../Common/defines.h"
#include "../Models/proxysettings.h"

namespace Conectivity {
    static size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data) {
        (void)ptr;
        (void)data;
        /* we are not interested in the headers itself,
         so we only return the size we would have saved ... */
        return (size_t)(size * nmemb);
    }

    std::string string_format(const std::string fmt, ...) {
        int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
        std::string str;
        va_list ap;
        while (1) {     // Maximum two passes on a POSIX system...
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
            va_end(ap);
            if (n > -1 && n < size) {  // Everything worked
                str.resize(n);
                return str;
            }
            if (n > -1)  // Needed size returned
                size = n + 1;   // For null char
            else
                size *= 2;      // Guess at a larger size (OS specific)
        }

        return str;
    }

    static
    void dump(const char *text,
              unsigned char *ptr, size_t size) {
        std::stringstream ss;
        size_t i;
        size_t c;
        unsigned int width = 0x10;

        ss << string_format("%s, %10.10ld bytes (0x%8.8lx)\n",
                text, (long)size, (long)size);

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
        LOG_DEBUG << QString::fromStdString(ss.str()).trimmed();
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
          LOG_INFO << QString::fromStdString(string_format("== Info: %s", data)).trimmed();
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

    ContextValidationResult isContextValid(Conectivity::UploadContext *context) {
        bool result = false;
        CURL *curlHandle = NULL;
        QString host = sanitizeHost(context->m_Host);
        const QString &remoteUrl = host;

        curl_global_init(CURL_GLOBAL_ALL);
        curlHandle = curl_easy_init();

        CURLcode r = CURLE_GOT_NOTHING;

        fillCurlOptions(curlHandle, context, remoteUrl);

        curl_easy_setopt(curlHandle, CURLOPT_UPLOAD, 0L);
        curl_easy_setopt(curlHandle, CURLOPT_DIRLISTONLY, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, throw_away);
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_DEBUGFUNCTION, my_trace);

        LOG_DEBUG << "About to check credentials for" << host;
        r = curl_easy_perform(curlHandle);

        result = (CURLE_OK == r);

        if (!result) {
            LOG_INFO << "Credentials checking error:" << curl_easy_strerror(r);
        }

        curl_easy_cleanup(curlHandle);
        curl_global_cleanup();

        LOG_INFO << "Credentials checking finished" << host;
        delete context;

        ContextValidationResult cvr;
        cvr.m_Host = host;
        cvr.m_Result = result;

        return cvr;
    }
}
