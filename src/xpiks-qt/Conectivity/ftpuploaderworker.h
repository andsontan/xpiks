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

#ifndef FTPUPLOADERWORKER_H
#define FTPUPLOADERWORKER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <memory>

class QSemaphore;

namespace Encryption {
    class SecretsManager;
}

namespace Models {
    class UploadInfo;
}

namespace Conectivity {
    class UploadBatch;

    class FtpUploaderWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit FtpUploaderWorker(QSemaphore *uploadSemaphore,
                                   const std::shared_ptr<UploadBatch> &batch,
                                   const std::shared_ptr<Models::UploadInfo> &uploadInfo,
                                   QObject *parent = 0);

        virtual ~FtpUploaderWorker();

    signals:
        void uploadStarted();
        void progressChanged(double oldPercents, double newPercents);
        void uploadFinished(bool anyErrors);
        void stopped();
        void workerCancelled();
        void transferFailed(const QString &filename, const QString &host);

    public slots:
        void process();
        void progressChangedHandler(double oldPercents, double newPercents);

    private:
        void doUpload();

    private:
        QSemaphore *m_UploadSemaphore;
        std::shared_ptr<UploadBatch> m_UploadBatch;
        std::shared_ptr<Models::UploadInfo> m_UploadInfo;
        QVector<QString> m_FailedTransfers;
    };
}

#endif // FTPUPLOADERWORKER_H
