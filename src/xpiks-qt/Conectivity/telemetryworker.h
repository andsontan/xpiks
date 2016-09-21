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

#ifndef TELEMETRYWORKER_H
#define TELEMETRYWORKER_H

#include <QObject>
#include "../Common/itemprocessingworker.h"
#include "analyticsuserevent.h"

class QNetworkReply;

namespace Conectivity {
    class TelemetryWorker : public QObject, public Common::ItemProcessingWorker<AnalyticsUserEvent>
    {
        Q_OBJECT
    public:
        TelemetryWorker(const QString &userAgent, const QString &reportingEndpoint, const QString &interfaceLanguage);

    protected:
        virtual bool initWorker();
        virtual void processOneItem(std::shared_ptr<AnalyticsUserEvent> &item);

    private:
        bool sendOneReport(const QString &resource, const QString &payload);

    protected:
        virtual void notifyQueueIsEmpty() { emit queueIsEmpty(); }
        virtual void workerStopped() { emit stopped(); }

    public slots:
        void process() { doWork(); }
        void cancel() { stopWorking(); }

    signals:
        void stopped();
        void queueIsEmpty();
        void cancelAllQueries();

    private:
        QString m_UserAgentId;
        QString m_ReportingEndpoint;
        QString m_InterfaceLanguage;
    };
}

#endif // TELEMETRYWORKER_H
