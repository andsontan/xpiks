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
#include <QCoreApplication>
#include <QFileInfo>
#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <curl/curl.h>
#include "ftphelpers.h"
#include "../Common/defines.h"
#include "uploadbatch.h"

#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 2

namespace Conectivity {

    /* this is how the CURLOPT_XFERINFOFUNCTION callback works */
    static int xferinfo(void *p,
                        curl_off_t dltotal, curl_off_t dlnow,
                        curl_off_t ultotal, curl_off_t ulnow)
    {
        Q_UNUSED(dltotal);
        Q_UNUSED(dlnow);

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

            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        int result = progressReporter->cancelRequested() ? 1 : 0;
        if (result) {
            LOG_DEBUG << "Cancelling upload from the progress callback...";
        }

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
        LOG_INFO << filepath << "-->" << remoteUrl;
        bool result = false;

        FILE *f;
        long uploaded_len = 0;
        CURLcode r = CURLE_GOT_NOTHING;
#ifdef Q_OS_WIN
        struct _stat64i32 file_info;
#else
        struct stat file_info;
#endif
        int c;
        curl_off_t fsize;

        /* get the file size of the local file */
#ifdef Q_OS_WIN
        if (_wstat(filepath.toStdWString().c_str(), &file_info)) {
            LOG_WARNING << "Failed to stat file" << filepath;
            return result;
        }
#else
        if (stat(filepath.toLocal8Bit().data(), &file_info)) {
            LOG_WARNING << "Failed to stat file" << filepath;
            return result;
        }
#endif

        fsize = (curl_off_t) file_info.st_size;

#ifdef Q_OS_WIN
        f = _wfopen(filepath.toStdWString().c_str(), L"rb");
#else
        f = fopen(filepath.toLocal8Bit().data(), "rb");
#endif
        if (f == NULL) {
            LOG_WARNING << "Failed to open file" << filepath;
            return result;
        }

        fillCurlOptions(curlHandle, context, remoteUrl);
        setCurlProgressCallback(curlHandle, progressReporter);
        curl_easy_setopt(curlHandle, CURLOPT_READDATA, f);
        curl_easy_setopt(curlHandle, CURLOPT_INFILESIZE_LARGE,
                             (curl_off_t)fsize);
        curl_easy_setopt(curlHandle, CURLOPT_HEADERDATA, &uploaded_len);

#ifdef QT_DEBUG
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
#endif

        for (c = 0; (r != CURLE_OK) && (c < context->m_RetriesCount); c++) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            if (r == CURLE_ABORTED_BY_CALLBACK) {
                LOG_INFO << "Upload aborted by user...";
                break;
            }

            /* are we resuming? */
            if (c) { /* yes */
                LOG_INFO << "Attempting to resume upload" << uploaded_len << "try #" << c;
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
                    LOG_WARNING << "Attempt failed! Curl error:" << curl_easy_strerror(r);
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
        }

        fclose(f);

        result = (r == CURLE_OK);

        if (!result) {
            LOG_WARNING << "Upload failed! Curl error:" << curl_easy_strerror(r);
        }

        return result;
    }

    QString generateRemoteAddress(const QString &host, const QString &filepath, UploadContext *context) {
        QFileInfo fi(filepath);
        QString extension = fi.completeSuffix().toLower();
        QString filename = fi.fileName();
        QString result;

        // TODO: refactor to ::isSupportedVectorExtension()
        if (extension == QLatin1String("eps") ||
                extension == QLatin1String("ai")) {
            QString vectorsDir = context->m_DirForVectors.trimmed();

            if (!vectorsDir.isEmpty()) {
                const QChar slash('/');
                if (vectorsDir.startsWith(slash)) {
                    vectorsDir.remove(0, 1);
                }

                if (!vectorsDir.endsWith(slash)) {
                    vectorsDir.append(slash);
                }

                result = host + vectorsDir + filename;
            } else {
                result = host + filename;
            }
        } else {
            // TODO: assume it's image, but add video later here
            QString imagesDir = context->m_DirForImages.trimmed();
            if (!imagesDir.isEmpty()) {
                const QChar slash('/');
                if (imagesDir.startsWith(slash)) {
                    imagesDir.remove(0, 1);
                }

                if (!imagesDir.endsWith(slash)) {
                    imagesDir.append(slash);
                }

                result = host + imagesDir + filename;
            } else {
                result = host + filename;
            }
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
        if (fabs(ultotal) > 1e-6) {
            double progress = ulnow * 100.0 / ultotal;
            emit progressChanged(progress);
        }
    }

    void CurlProgressReporter::cancelHandler() {
        m_Cancel = true;
        LOG_INFO << "Cancelled in the progress reporter...";
    }

    CurlFtpUploader::CurlFtpUploader(const std::shared_ptr<UploadBatch> &batchToUpload, QObject *parent) :
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

        if (m_Cancel) {
            LOG_WARNING << "Cancelled before upload." << context->m_Host;
            return;
        }

        const QStringList &filesToUpload = m_BatchToUpload->getFilesToUpload();
        int size = filesToUpload.size();

        QString host = sanitizeHost(context->m_Host);
        QString title = context->m_Title;

        // curl_global_init should be done from coordinator
        curlHandle = curl_easy_init();

        CurlProgressReporter progressReporter(curlHandle);
        QObject::connect(&progressReporter, SIGNAL(progressChanged(double)),
                         this, SLOT(reportCurrentFileProgress(double)));
        QObject::connect(this, SIGNAL(cancelCurrentUpload()), &progressReporter, SLOT(cancelHandler()));

        // temporary do not emit started signal: not used
        //emit uploadStarted();
        LOG_INFO << "Uploading" << size << "file(s) started for" << host << "Passive mode =" << context->m_UsePassiveMode;

        for (int i = 0; i < size; ++i) {

            if (m_Cancel) {
                LOG_WARNING << "Cancelled. Breaking..." << host;
                break;
            }

            reportCurrentFileProgress(0.0);

            const QString &filepath = filesToUpload.at(i);
            QString remoteUrl = generateRemoteAddress(host, filepath, context);
            bool uploadSuccess = false;

            try {
                uploadSuccess = uploadFile(curlHandle, context, &progressReporter, filepath, remoteUrl);
            } catch (...) {
                LOG_WARNING << "CRASHED for file" << filepath << "for host" << host;
            }

            if (!uploadSuccess) {
                anyErrors = true;
                emit transferFailed(filepath, host);
            }

            // TODO: only update progress of not-failed uploads
            if (uploadSuccess) {
                m_UploadedCount++;
            }
        }

        reportCurrentFileProgress(0.0);

        emit uploadFinished(anyErrors);
        LOG_INFO << "Uploading finished for" << host;

        curl_easy_cleanup(curlHandle);
        // curl_global_cleanup should be done from coordinator
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

