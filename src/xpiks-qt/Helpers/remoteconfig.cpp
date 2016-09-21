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
#include <QThread>
#include "../Common/defines.h"
#include "../Conectivity/simplecurlrequest.h"

namespace Helpers {
    RemoteConfig::RemoteConfig(QObject *parent):
        QObject(parent)
    {
    }

    RemoteConfig::~RemoteConfig() {
    }

    void RemoteConfig::requestInitConfig(const QString &configUrl) {
        m_ConfigUrl = configUrl;
        LOG_DEBUG << m_ConfigUrl;

        Conectivity::SimpleCurlRequest *request = new Conectivity::SimpleCurlRequest(configUrl);
        QThread *thread = new QThread();

        request->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), request, SLOT(process()));
        QObject::connect(request, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(request, SIGNAL(stopped()), request, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(request, SIGNAL(requestFinished(bool)), this, SLOT(requestFinishedHandler(bool)));

        thread->start(QThread::LowPriority);
        LOG_INFO << "Started request thread for" << configUrl;
    }

    void RemoteConfig::requestFinishedHandler(bool success) {
        LOG_INFO << "success:" << success;

        Conectivity::SimpleCurlRequest *request = qobject_cast<Conectivity::SimpleCurlRequest *>(sender());

        if (success) {
            QJsonParseError error;

            auto responseData = request->getResponseData();
            LOG_INTEGR_TESTS_OR_DEBUG << responseData;

            m_Config = QJsonDocument::fromJson(responseData, &error);

            if (error.error == QJsonParseError::NoError) {
                emit configArrived();
            } else {
                LOG_WARNING << "Failed to parse remote json" << error.errorString();
            }
        }

        request->dispose();
    }
}
