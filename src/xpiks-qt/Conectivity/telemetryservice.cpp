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

#include <QTime>
#include <QSettings>
#include <QRegExp>
#include <QUrl>
#include <QUrlQuery>
#include <QSysInfo>
#include <QNetworkRequest>
#include <QByteArray>
#include <QNetworkReply>
#include "telemetryservice.h"
#include "analyticsuserevent.h"
#include "../Common/defines.h"
#include "../Models/settingsmodel.h"
#include "../Common/version.h"

namespace Conectivity {
    TelemetryService::TelemetryService(const QString &userId, bool telemetryEnabled, QObject *parent) :
        QObject(parent),
        m_NetworkManager(this),
        m_UserAgentId(userId),
        m_InterfaceLanguage("en_US"),
        m_TelemetryEnabled(telemetryEnabled)
    {
        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void TelemetryService::reportAction(UserAction action) {
#if defined(QT_NO_DEBUG) && defined(TELEMETRY_ENABLED)
        if (m_TelemetryEnabled)
        {
            doReportAction(action);
        }
        else
#endif
            Q_UNUSED(action);
    }

    void TelemetryService::changeReporting(bool value) {
#if defined(QT_NO_DEBUG) && defined(TELEMETRY_ENABLED)
        if (m_TelemetryEnabled != value) {
            m_TelemetryEnabled = value;

            if (m_TelemetryEnabled) {
                LOG_DEBUG << "Telemetry enabled by setting";
            } else {
                LOG_DEBUG << "Telemetry disabled by setting";
                doReportAction(UserActionTurnOffTelemetry);
            }
        }
#else
        LOG_INFO << "Setting telemetry to" << value << "but it is disabled at compile time";
#endif
    }

    void TelemetryService::setEndpoint(const QString &endpoint) {
        m_ReportingEndpoint = endpoint;
    }

    void TelemetryService::doReportAction(UserAction action) {
        AnalyticsUserEvent userEvent(action);
        LOG_INFO << "Reporting action" << userEvent.getActionString();

        QUrlQuery query;
        buildQuery(userEvent, query);

        QString customVarsStr = QString::fromLatin1("{\"1\":[\"OS_type\",\"%1\"],\"2\":[\"OS_version\",\"%2\"],\"3\":[\"Xpiks_version\",\"%3\"],\"4\":[\"UI_language\",\"%4\"]}");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
        query.addQueryItem(QLatin1String("_cvar"),
                           customVarsStr
                           .arg(QSysInfo::productType())
                           .arg(QSysInfo::productVersion())
                           .arg(XPIKS_VERSION_STRING)
                           .arg(m_InterfaceLanguage));
#else
        query.addQueryItem(QLatin1String("_cvar"),
                           customVarsStr
#ifdef Q_OS_WIN
                           .arg(QString("windows"))
#elsif Q_OS_DARWIN
                           .arg(QString("osx"))
#else
                           .arg(QString("Linux QT<5.4"))
#endif
                           .arg(QString("-"))
                           .arg(XPIKS_VERSION_STRING)
                           .arg(m_InterfaceLanguage));
#endif

        QUrl reportingUrl;
        reportingUrl.setUrl(m_ReportingEndpoint);
        reportingUrl.setQuery(query);


#ifdef QT_DEBUG
        LOG_DEBUG << "Telemetry request" << reportingUrl;
#endif

        QNetworkRequest request(reportingUrl);

#if defined(Q_OS_DARWIN)
        request.setRawHeader(QString("User-Agent").toLocal8Bit(), QString("Mozilla/5.0 (Macintosh; Mac OS X %2; rv:1.1) Qt Xpiks/1.1")
                .arg(QSysInfo::productVersion()).toLocal8Bit());
#elif defined(Q_OS_WIN)
        request.setRawHeader(QString("User-Agent").toLocal8Bit(), QString("Mozilla/5.0 (Windows %2; rv:1.1) Qt Xpiks/1.1")
                .arg(QSysInfo::productVersion()).toLocal8Bit());
#elif defined(Q_OS_LINUX)
        request.setRawHeader(QString("User-Agent").toLocal8Bit(), QString("Mozilla/5.0 (Linux %2; rv:1.1) Qt Xpiks/1.1")
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
                .arg(QSysInfo::productVersion()).toLocal8Bit());
#else
                .arg("?").toLocal8Bit());
#endif
#endif

        QNetworkReply *reply = m_NetworkManager.get(request);
        QObject::connect(this, SIGNAL(cancelAllQueries()),
                         reply, SLOT(abort()));
    }

    void TelemetryService::buildQuery(AnalyticsUserEvent &userEvent, QUrlQuery &query) {
        query.addQueryItem(QLatin1String("idsite"), QLatin1String("1"));
        query.addQueryItem(QLatin1String("rec"), QLatin1String("1"));
        query.addQueryItem(QLatin1String("url"), QString("/client/%1").arg(userEvent.getActionString()));
        query.addQueryItem(QLatin1String("action_name"), userEvent.getActionString());
        query.addQueryItem(QLatin1String("_id"), m_UserAgentId);
        query.addQueryItem(QLatin1String("rand"), QString::number(qrand()));
        query.addQueryItem(QLatin1String("apiv"), QLatin1String("1"));
        query.addQueryItem(QLatin1String("h"), QString::number(userEvent.getHour()));
        query.addQueryItem(QLatin1String("m"), QString::number(userEvent.getMinute()));
        query.addQueryItem(QLatin1String("s"), QString::number(userEvent.getSecond()));
        query.addQueryItem(QLatin1String("send_image"), QLatin1String("0"));
    }

    void TelemetryService::replyReceived(QNetworkReply *networkReply) {
        if (networkReply->error() != QNetworkReply::NoError) {
            // TODO: add tracking of failed items

            LOG_WARNING << "Failed to process a telemetry report." << networkReply->errorString();;
        }

        networkReply->deleteLater();
    }
}

