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

#ifndef UPLOADWORKER_H
#define UPLOADWORKER_H

#include <QObject>
#include <QProcess>

class QSemaphore;
class QTimer;
class QRegExp;
class QString;

namespace Encryption {
    class SecretsManager;
}

namespace Models {
    class UploadInfo;
}

namespace Helpers {
    class UploadItem;

    class UploadWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit UploadWorker(UploadItem *uploadItem, const Encryption::SecretsManager *secretsManager,
                              QSemaphore *uploadSemaphore, int delay,
                              QObject *parent = 0);
        virtual ~UploadWorker();

    signals:
        void stopped();
        void finished(bool success);
        void error(QString err);
        void percentChanged(double newPercent, double oldPercent);

    public slots:
        void process();
        void cancel();

    private slots:
        void innerProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void uploadOutputReady();
        void onTimerTimeout();

    private:
        QString createCurlCommand(Models::UploadInfo *uploadInfo, const QStringList &filesToUpload, int maxSeconds) const;
        void initializeUploadEntities();
        int parsePercent(QString &curlOutput)const;
        void emitFinishSignals(bool success);
        void updateUploadItemPercent(int percent);

    private:
        UploadItem *m_UploadItem;
        const Encryption::SecretsManager *m_SecretsManager;
        QSemaphore *m_UploadSemaphore;
        QProcess *m_CurlProcess;
        QTimer *m_Timer;
        QString m_Host;
        QRegExp m_PercentRegexp;
        int m_Delay;
        volatile double m_PercentDone;
        volatile int m_FilesUploaded;
        int m_OverallFilesCount;
        volatile bool m_Cancelled;
    };
}

#endif // UPLOADWORKER_H
