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

#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <QObject>

namespace Models {
    class SettingsModel;
}

namespace Conectivity {
    class UpdatesCheckerWorker;

    class UpdateService : public QObject
    {
        Q_OBJECT
    public:
        UpdateService(Models::SettingsModel *settingsModel);

    public:
        void startChecking();
        void stopChecking();

    private:
        void doStartChecking();
        void updateSettings();

    private slots:
        void workerFinished();
        void updateDownloadedHandler(const QString &updatePath, int version);

    signals:
        void updateAvailable(QString updateLink);
        void updateDownloaded();

    private:
        void saveUpdateInfo() const;

    private:
        Conectivity::UpdatesCheckerWorker *m_UpdatesCheckerWorker;
        Models::SettingsModel *m_SettingsModel;
        QString m_PathToUpdate;
        int m_AvailableVersion;
        volatile bool m_UpdateAvailable;
    };
}

Q_DECLARE_METATYPE(Conectivity::UpdateService*)

#endif // UPDATESERVICE_H
