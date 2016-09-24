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

#ifndef FTPCOORDINATOR_H
#define FTPCOORDINATOR_H

#include <QObject>
#include <QVector>
#include <QSemaphore>
#include "../Common/baseentity.h"
#include "iftpcoordinator.h"
#include <QAtomicInt>
#include <QMutex>
#include "../Models/settingsmodel.h"

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
    class ProxySettings;
}

namespace Conectivity {
    class UploadContext;

    class FtpCoordinator :
            public QObject,
            public Common::BaseEntity,
            public IFtpCoordinator
    {
        Q_OBJECT
    public:
        explicit FtpCoordinator(int maxParallelUploads, QObject *parent = 0);

    public:
        // IFTPCOORDINATOR
        virtual void uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworksToUpload,
                            std::vector<std::shared_ptr<Models::UploadInfo> > &uploadInfos);
        virtual void cancelUpload();

    signals:
        void uploadStarted();
        void cancelAll();
        void uploadFinished(bool anyError);
        void overallProgressChanged(double percentDone);
        void transferFailed(const QString &filepath, const QString &host);

    private slots:
        void workerProgressChanged(double oldPercents, double newPercents);
        void workerFinished(bool anyErrors);

    private:
        void initUpload(size_t uploadBatchesCount);
        void finalizeUpload();

    private:
        QMutex m_WorkerMutex;
        QSemaphore m_UploadSemaphore;
        double m_OverallProgress;
        QAtomicInt m_FinishedWorkersCount;
        volatile size_t m_AllWorkersCount;
        volatile bool m_AnyFailed;
    };
}

#endif // FTPCOORDINATOR_H
