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

#ifndef CURLFTPUPLOADER_H
#define CURLFTPUPLOADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <memory>
#include "uploadcontext.h"

namespace Conectivity {
    class UploadBatch;

    class CurlProgressReporter : public QObject {
        Q_OBJECT
    public:
        CurlProgressReporter(void *curl);

    public:
        void updateProgress(double ultotal, double ulnow);
        void *getCurl() const { return m_Curl; }
        double getLastTime() const { return m_LastTime; }
        void setLastTime(double value) { m_LastTime = value; }
        bool cancelRequested() const { return m_Cancel; }

    signals:
        void progressChanged(double percentsDone);

    public slots:
        void cancelHandler();

    private:
        double m_LastTime;
        void *m_Curl;
        volatile bool m_Cancel;
    };

    class CurlFtpUploader : public QObject
    {
        Q_OBJECT
    public:
        explicit CurlFtpUploader(const std::shared_ptr<UploadBatch> &batchToUpload, QObject *parent = 0);

    public:
        void uploadBatch();

    signals:
        void uploadStarted();
        void progressChanged(double prevPercents, double newPercents);
        void uploadFinished(bool anyErrors);
        void transferFailed(const QString &filepath, const QString &host);
        void cancelCurrentUpload();

    public slots:
        void cancel();

    private slots:
        void reportCurrentFileProgress(double percent);

    private:
        std::shared_ptr<UploadBatch> m_BatchToUpload;
        volatile int m_UploadedCount;
        volatile bool m_Cancel;
        double m_LastPercentage;
        int m_TotalCount;
    };
}

#endif // CURLFTPUPLOADER_H
