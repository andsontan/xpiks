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

#ifndef UPDATESCHECKERWORKER_H
#define UPDATESCHECKERWORKER_H

#include <QObject>

namespace Models {
    class SettingsModel;
}

namespace Conectivity {
    struct UpdateCheckResult {
        QString m_UpdateURL;
        QString m_Checksum;
        int m_Version;
    };

    class UpdatesCheckerWorker : public QObject
    {
        Q_OBJECT
    public:
        UpdatesCheckerWorker(Models::SettingsModel *settingsModel);
        virtual ~UpdatesCheckerWorker();

    private:
        void initWorker();
        void processOneItem();
        bool checkForUpdates(UpdateCheckResult &result);

    public slots:
        void process();

    signals:
        void stopped();
        void updateAvailable(QString updateLink);
        void updateDownloaded(const QString &updatePath, int version);
        void requestFinished();

    private:
        Models::SettingsModel *m_SettingsModel;
        QString m_UpdatesDirectory;
    };
}

#endif // UPDATESCHECKERWORKER_H
