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

#include "remoteconfig.h"

namespace Helpers {
    RemoteConfig::RemoteConfig() {
        m_NetworkManager = new QNetworkAccessManager();

        QObject::connect(m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    RemoteConfig::~RemoteConfig() {
        m_NetworkManager->deleteLater();
    }

    void RemoteConfig::requestInitConfig(const QString &configUrl) {
        m_ConfigUrl = configUrl;

        LOG_DEBUG << m_ConfigUrl;

        QUrl url;
        url.setUrl(m_ConfigUrl);

        QNetworkRequest request(url);
        QNetworkReply *reply = m_NetworkManager->get(request);
        Q_UNUSED(reply);
    }

    void RemoteConfig::replyReceived(QNetworkReply *networkReply) {
        LOG_DEBUG << "#";

        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonParseError error;
            m_Config = QJsonDocument::fromJson(networkReply->readAll(), &error);

            if (error.error == QJsonParseError::NoError) {
                emit configArrived();
            } else {
                LOG_WARNING << "Failed to parse remote json" << error.errorString();
            }
        } else {
            LOG_WARNING << "Config download failed:" << networkReply->errorString();
        }

        networkReply->deleteLater();
    }
}
