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
#include <QVector>
#include <QSharedPointer>

namespace Conectivity {
    struct UploadContext {
        QString m_Host;
        QString m_Username;
        QString m_Password;
        bool m_UsePassiveMode;
        int m_RetriesCount;
        int m_TimeoutSeconds;
        // proxy info
    };

    class UploadBatch {
    public:
        UploadBatch(UploadContext *context, const QVector<QString> &filesList):
            m_FilesList(filesList),
            m_UploadContext(context)
        {}

        ~UploadBatch() {
            m_UploadContext.clear();
        }

    public:
        const QVector<QString> &getFilesToUpload() const { return m_FilesList; }
        UploadContext *getContext() const { return m_UploadContext.data(); }
        
    private:
        QVector<QString> m_FilesList;
        QSharedPointer<UploadContext> m_UploadContext;
    };

    class CurlFtpUploader : public QObject
    {
        Q_OBJECT
    public:
        explicit CurlFtpUploader(UploadBatch *batchToUpload, QObject *parent = 0);

    public:
        void uploadBatch();

    signals:
        void uploadStarted();
        void progressChanged(int percents);
        void uploadFinished();

    public slots:
        void cancel();

    private:
        UploadBatch *m_BatchToUpload;
        QVector<int> m_FailedIndices;
        volatile bool m_Cancel;
    };
}

#endif // CURLFTPUPLOADER_H
