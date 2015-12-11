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

#ifndef TELEMETRYSERVICE_H
#define TELEMETRYSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>
#include "analyticsuserevent.h"

namespace Conectivity {
    class TelemetryService : public QObject {
        Q_OBJECT
    public:
        TelemetryService(const QString &userId, const QString &endpoint, QObject *parent=NULL);

    public:
        void reportAction(UserAction action);

    private:
        void doReportAction(UserAction action);

    signals:
        void cancelAllQueries();

    private slots:
        void replyReceived(QNetworkReply *reply);

    private:
        QNetworkAccessManager m_NetworkManager;
        QString m_ReportingEndpoint;
        QString m_UserAgentId;
    };
}

#endif // TELEMETRYSERVICE_H
