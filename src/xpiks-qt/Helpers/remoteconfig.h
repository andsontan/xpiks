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

#ifndef RemoteConfig_H
#define RemoteConfig_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QJsonDocument>
#include "../Common/defines.h"

namespace Helpers {
    class RemoteConfig : public QObject {
        Q_OBJECT
    public:
        RemoteConfig(const QString &url);
        virtual ~RemoteConfig();

    public:
        void requestInitConfig();
        const QJsonDocument& getConfig() const { return m_Config; }

    signals:
        void updateIsReady();

    private slots:
        void replyReceived(QNetworkReply *networkReply);

    private:
        QString m_Url;
        QNetworkAccessManager *m_NetworkManager;
        QJsonDocument m_Config;
    };
}

#endif // RemoteConfig_H
