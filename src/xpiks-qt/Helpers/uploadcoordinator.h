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

#ifndef UPLOADCOORDINATOR_H
#define UPLOADCOORDINATOR_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QMutex>
#include <QThread>
#include <QVector>
#include <QSemaphore>

namespace Models {
    class ArtworkMetadata;
    class UploadInfo;
    class SettingsModel;
}

namespace Encryption {
    class SecretsManager;
}

namespace Helpers {
    class UploadInfo;
    class UploadItem;

    class UploadCoordinator : public QObject
    {
        Q_OBJECT
    public:
        UploadCoordinator(int maxParallelUploads, QObject *parent = 0):
            QObject(parent),
            m_UploadSemaphore(maxParallelUploads),
            m_MaxParallelUploads(maxParallelUploads),
            m_AnyFailed(false)
        {}

        ~UploadCoordinator() {}

    public:
        void uploadArtworks(const QVector<Models::ArtworkMetadata *> &artworkList,
                            const QVector<Models::UploadInfo *> &uploadInfos,
                            bool includeVector,
                            const Encryption::SecretsManager *secretsManager,
                            const Models::SettingsModel *settings);

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
        void doRunUpload(const QVector<UploadItem *> &uploadItems, const Encryption::SecretsManager *secretsManager);
        void extractFilePathes(const QVector<Models::ArtworkMetadata*> &artworkList,
                               QStringList &filePathes, QStringList &zipsPathes, bool includeVector) const;
        void stopThreads();

    private:
        QMutex m_Mutex;
        QMutex m_PercentMutex;
        QSemaphore m_UploadSemaphore;
        QVector<QThread *> m_UploadThreads;
        int m_WorkersCount;
        int m_AllWorkersCount;
        int m_MaxParallelUploads;
        bool m_AnyFailed;
        double m_PercentDone;
    };
}

#endif // UPLOADCOORDINATOR_H
