/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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
#include <QString>

namespace Encryption {
    class SecretsManager;
}

namespace Helpers {
    class UploadItem;

    class UploadWorker : public QObject
    {
        Q_OBJECT
    public:
        explicit UploadWorker(UploadItem *uploadItem, const Encryption::SecretsManager *secretsManager, QObject *parent = 0);
        ~UploadWorker();

    public:
        bool getSuccessStatus() const { return m_SuccessStatus; }

    signals:
        void stopped();
        void finished(bool success);
        void error(QString err);

    public slots:
        void process();
        void cancel();

    private:
        UploadItem *m_UploadItem;
        const Encryption::SecretsManager *m_SecretsManager;
        QProcess m_CurlProcess;
        bool m_SuccessStatus;
    };
}

#endif // UPLOADWORKER_H
