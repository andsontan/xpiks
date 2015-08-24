/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef UPLOADCOORDINATOR_H
#define UPLOADCOORDINATOR_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QMutex>
#include <QThread>
#include <QSemaphore>

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
}

namespace Encryption {
    class SecretsManager;
}

// TODO: move to configs and settings (values from 1 to 4)
#define MAX_PARALLEL_UPLOAD 2

namespace Helpers {
    class UploadInfo;
    class UploadItem;

    class UploadCoordinator : public QObject
    {
        Q_OBJECT
    public:
        UploadCoordinator(QObject *parent = 0):
            QObject(parent),
            m_UploadSemaphore(MAX_PARALLEL_UPLOAD)
        {}

        ~UploadCoordinator() {}

    public:
        void uploadArtworks(const QList<Models::ArtworkMetadata *> &artworkList,
                            const QList<Models::UploadInfo *> &uploadInfos,
                            bool includeEPS,
                            const Encryption::SecretsManager *secretsManager);

        void cancelUpload();

    signals:
        void cancelAll();
        void uploadStarted();
        void itemFinished(bool success);
        void uploadFinished(bool success);
        void percentChanged(double percent);

    public slots:
        void workerFinished(bool success);
        void percentReported(double newPercent, double oldPercent);

    private:
        void doRunUpload(const QList<UploadItem *> &uploadItems, const Encryption::SecretsManager *secretsManager);
        void extractFilePathes(const QList<Models::ArtworkMetadata*> &artworkList,
                               QStringList &filePathes, QStringList &zipsPathes, bool includeEPS) const;
        void stopThreads();

    private:
        QMutex m_Mutex;
        QMutex m_PercentMutex;
        QSemaphore m_UploadSemaphore;
        QList<QThread *> m_UploadThreads;
        int m_WorkersCount;
        int m_AllWorkersCount;
        bool m_AnyFailed;
        double m_PercentDone;
    };
}

#endif // UPLOADCOORDINATOR_H
