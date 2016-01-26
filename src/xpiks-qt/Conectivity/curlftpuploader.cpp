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

#include "curlftpuploader.h"
#include <QDebug>
#include <QFileInfo>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../../libcurl/include/curl/curl.h"

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 1

namespace Conectivity {
    /* The MinGW headers are missing a few Win32 function definitions,
       you shouldn't need this if you use VC++ */
#if defined(__MINGW32__) && !defined(__MINGW64__)
    int __cdecl _snscanf(const char * input, size_t length, const char * format, ...);
#endif


    /* parse headers for Content-Length */
    size_t getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        int r;
        long len = 0;

#if defined(Q_OS_WIN)
        /* _snscanf() is Win32 specific */
        r = _snscanf((const char*)ptr, size * nmemb, "Content-Length: %ld\n", &len);
#else
        r = std::sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
#endif

        if (r) /* Microsoft: we don't read the specs */
            *((long *) stream) = len;

        return size * nmemb;
    }

    /* discard downloaded data */
    size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        Q_UNUSED(stream);
        Q_UNUSED(ptr);
        return size * nmemb;
    }

    /* read data to upload */
    size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        FILE *f = (FILE*)stream;
        size_t n;

        if (ferror(f))
            return CURL_READFUNC_ABORT;

        n = fread(ptr, size, nmemb, f) * size;

        return n;
    }

    void fillCurlOptions(CURL *curlHandle, UploadContext *context, const QString &remoteUrl) {
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

    /* this is how the CURLOPT_XFERINFOFUNCTION callback works */
    static int xferinfo(void *p,
                        curl_off_t dltotal, curl_off_t dlnow,
                        curl_off_t ultotal, curl_off_t ulnow)
    {
        CurlProgressReporter *progressReporter = (CurlProgressReporter *)p;
        CURL *curl = progressReporter->getCurl();
        double curtime = 0;

        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);

        /* under certain circumstances it may be desirable for certain functionality
         to only run every N seconds, in order to do this the transaction time can
         be used */
        if ((curtime - progressReporter->getLastTime()) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
            progressReporter->setLastTime(curtime);
            progressReporter->updateProgress((double)ultotal, (double)ulnow);
        }

        int result = progressReporter->cancelRequested() ? 1 : 0;
        return result;
    }

    /* for libcurl older than 7.32.0 (CURLOPT_PROGRESSFUNCTION) */
    static int older_progress(void *p,
                              double dltotal, double dlnow,
                              double ultotal, double ulnow)
    {
        return xferinfo(p,
                        (curl_off_t)dltotal,
                        (curl_off_t)dlnow,
                        (curl_off_t)ultotal,
                        (curl_off_t)ulnow);
    }

    void setCurlProgressCallback(CURL *curlHandle, CurlProgressReporter *progressReporter) {
        curl_easy_setopt(curlHandle, CURLOPT_PROGRESSFUNCTION, older_progress);
        /* pass the struct pointer into the progress function */
        curl_easy_setopt(curlHandle, CURLOPT_PROGRESSDATA, progressReporter);

#if LIBCURL_VERSION_NUM >= 0x072000
        /* xferinfo was introduced in 7.32.0, no earlier libcurl versions will
           compile as they won't have the symbols around.

           If built with a newer libcurl, but running with an older libcurl:
           curl_easy_setopt() will fail in run-time trying to set the new
           callback, making the older callback get used.

           New libcurls will prefer the new callback and instead use that one even
           if both callbacks are set. */

        curl_easy_setopt(curlHandle, CURLOPT_XFERINFOFUNCTION, xferinfo);
        /* pass the struct pointer into the xferinfo function, note that this is
              an alias to CURLOPT_PROGRESSDATA */
        curl_easy_setopt(curlHandle, CURLOPT_XFERINFODATA, progressReporter);
#endif

        curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 0L);
    }

    bool uploadFile(CURL *curlHandle, UploadContext *context, CurlProgressReporter *progressReporter,
                    const QString &filepath, const QString &remoteUrl) {
        bool result = false;

        FILE *f;
        long uploaded_len = 0;
        CURLcode r = CURLE_GOT_NOTHING;
        struct stat file_info;
        int c;
        curl_off_t fsize;

        /* get the file size of the local file */
        if (stat(filepath.toLocal8Bit().data(), &file_info)) {
            qWarning() << "Failed to open file" << filepath;
            return result;
        }

        fsize = (curl_off_t) file_info.st_size;

        f = fopen(filepath.toLocal8Bit().data(), "rb");
        if (f == NULL) {
            qWarning() << "Failed to open file" << filepath;
            return result;
        }

        fillCurlOptions(curlHandle, context, remoteUrl);
        setCurlProgressCallback(curlHandle, progressReporter);
        curl_easy_setopt(curlHandle, CURLOPT_READDATA, f);
        curl_easy_setopt(curlHandle, CURLOPT_INFILESIZE_LARGE,
                             (curl_off_t)fsize);
        curl_easy_setopt(curlHandle, CURLOPT_HEADERDATA, &uploaded_len);

        for (c = 0; (r != CURLE_OK) && (c < context->m_RetriesCount); c++) {
            /* are we resuming? */
            if (c) { /* yes */
                /* determine the length of the file already written */
                /*
               * With NOBODY and NOHEADER, libcurl will issue a SIZE
               * command, but the only way to retrieve the result is
               * to parse the returned Content-Length header. Thus,
               * getcontentlengthfunc(). We need discardfunc() above
               * because HEADER will dump the headers to stdout
               * without it.
               */
                curl_easy_setopt(curlHandle, CURLOPT_NOBODY, 1L);
                curl_easy_setopt(curlHandle, CURLOPT_HEADER, 1L);

                r = curl_easy_perform(curlHandle);
                if (r != CURLE_OK) {
                    continue;
                }

                curl_easy_setopt(curlHandle, CURLOPT_NOBODY, 0L);
                curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);

                fseek(f, uploaded_len, SEEK_SET);

                curl_easy_setopt(curlHandle, CURLOPT_APPEND, 1L);
            }
            else { /* no */
                curl_easy_setopt(curlHandle, CURLOPT_APPEND, 0L);
            }

            r = curl_easy_perform(curlHandle);

            if (r == CURLE_ABORTED_BY_CALLBACK) {
                qInfo() << "Upload aborted by user...";
                break;
            }
        }

        fclose(f);

        result = (r == CURLE_OK);

        if (!result) {
            qWarning() << "Upload failed! Curl error:" << curl_easy_strerror(r);
        }

        return result;
    }

    CurlProgressReporter::CurlProgressReporter(void *curl):
        QObject(),
        m_LastTime(0.0),
        m_Curl(curl),
        m_Cancel(false)
    {
    }

    void CurlProgressReporter::updateProgress(double ultotal, double ulnow) {
        if (fabs(ulnow - 0.0) > 1e-6) {
            double progress = ulnow * 100.0 / ultotal;
            emit progressChanged(progress);
        }
    }

    CurlFtpUploader::CurlFtpUploader(UploadBatch *batchToUpload, QObject *parent) :
        QObject(parent),
        m_BatchToUpload(batchToUpload),
        m_UploadedCount(0),
        m_Cancel(false),
        m_LastPercentage(0.0)
    {
        m_TotalCount = batchToUpload->getFilesToUpload().length();
    }

    void CurlFtpUploader::uploadBatch() {
        CURL *curlHandle = NULL;
        bool anyErrors = false;

        UploadContext *context = m_BatchToUpload->getContext();

        const QStringList &filesToUpload = m_BatchToUpload->getFilesToUpload();
        int size = filesToUpload.size();

        QString host = sanitizeHost(context->m_Host);

        // TODO: do not call this from thread
        //curl_global_init(CURL_GLOBAL_ALL);
        curlHandle = curl_easy_init();

        CurlProgressReporter progressReporter(curlHandle);
        QObject::connect(&progressReporter, SIGNAL(progressChanged(double)),
                         this, SLOT(reportCurrentFileProgress(double)));
        QObject::connect(this, SIGNAL(cancelCurrentUpload()), &progressReporter, SLOT(cancelHandler()));

        emit uploadStarted();
        qDebug() << "Uploading" << size << "file(s) started for" << host;

        for (int i = 0; i < size; ++i) {
            if (m_Cancel) {
                qWarning() << "CurlUploader: Cancelled. Breaking..." << host;
                break;
            }

            reportCurrentFileProgress(0.0);

            const QString &filepath = filesToUpload.at(i);
            QFileInfo fi(filepath);
            QString remoteUrl = host + fi.fileName();

            bool uploadSuccess = false;

            try {
                uploadSuccess = uploadFile(curlHandle, context, &progressReporter, filepath, remoteUrl);
            } catch (...) {
                qWarning() << "Upload CRASHED for file" << filepath;
            }

            if (!uploadSuccess) {
                anyErrors = true;
                emit transferFailed(filepath);
                //m_FailedIndices.append(i);
            }

            // TODO: only update progress of not-failed uploads
            if (uploadSuccess) {
                m_UploadedCount++;
            }
        }

        emit uploadFinished(anyErrors);
        qDebug() << "Uploading finished for" << host;

        curl_easy_cleanup(curlHandle);
        // TODO: do not call this from thread
        //curl_global_cleanup();
    }

    void CurlFtpUploader::cancel() {
        m_Cancel = true;
        emit cancelCurrentUpload();
    }

    void CurlFtpUploader::reportCurrentFileProgress(double percent) {
        double newProgress = m_UploadedCount*100.0 + percent;
        newProgress /= m_TotalCount;
        emit progressChanged(m_LastPercentage, newProgress);
        m_LastPercentage = newProgress;
    }
}

