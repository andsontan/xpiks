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

#include "telemetryworker.h"
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include <QSysInfo>
#include <QByteArray>
#include <QTime>
#include <QRegExp>
#include <curl/curl.h>
#include <string>
#include "../Common/version.h"

void buildQuery(std::shared_ptr<Conectivity::AnalyticsUserEvent> &userEvent, const QString &userAgent, QUrlQuery &query) {
    query.addQueryItem(QLatin1String("idsite"), QLatin1String("1"));
    query.addQueryItem(QLatin1String("rec"), QLatin1String("1"));
    query.addQueryItem(QLatin1String("url"), QString("/client/%1").arg(userEvent->getActionString()));
    query.addQueryItem(QLatin1String("action_name"), userEvent->getActionString());
    query.addQueryItem(QLatin1String("_id"), userAgent);
    query.addQueryItem(QLatin1String("rand"), QString::number(qrand()));
    query.addQueryItem(QLatin1String("apiv"), QLatin1String("1"));
    query.addQueryItem(QLatin1String("h"), QString::number(userEvent->getHour()));
    query.addQueryItem(QLatin1String("m"), QString::number(userEvent->getMinute()));
    query.addQueryItem(QLatin1String("s"), QString::number(userEvent->getSecond()));
    query.addQueryItem(QLatin1String("send_image"), QLatin1String("0"));
}

namespace Conectivity {
    TelemetryWorker::TelemetryWorker(const QString &userAgent, const QString &reportingEndpoint, const QString &interfaceLanguage):
        m_UserAgentId(userAgent),
        m_ReportingEndpoint(reportingEndpoint),
        m_InterfaceLanguage(interfaceLanguage)
    {
    }

    bool TelemetryWorker::initWorker() {
        return true;
    }

    void TelemetryWorker::processOneItem(std::shared_ptr<AnalyticsUserEvent> &item) {
        LOG_INFO << "Reporting action:" << item->getActionString();

        QUrlQuery query;
        buildQuery(item, m_UserAgentId, query);

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

#ifdef QT_DEBUG
        LOG_DEBUG << "Telemetry request:" << m_ReportingEndpoint << query.toString();
#endif

        sendOneReport(m_ReportingEndpoint, query.toString());
    }

    bool TelemetryWorker::sendOneReport(const QString &resource, const QString &payload) {
        CURL *curl_handle;
        CURLcode res;

        /* init the curl session */
        curl_handle = curl_easy_init();

        std::string resourceString = resource.toStdString();
        const char *url = resourceString.data();
        /* specify URL to get */
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);

        /* some servers don't like requests that are made without a user-agent
             field, so we provide one */
        QString userAgent;
#if defined(Q_OS_DARWIN)
        userAgent = QString("Mozilla/5.0 (Macintosh; Mac OS X %2; rv:1.1) Qt Xpiks/1.1")
                .arg(QSysInfo::productVersion());
#elif defined(Q_OS_WIN)
        userAgent = QString("Mozilla/5.0 (Windows %2; rv:1.1) Qt Xpiks/1.1")
                .arg(QSysInfo::productVersion());
#elif defined(Q_OS_LINUX)
        userAgent = QString("Mozilla/5.0 (Linux %2; rv:1.1) Qt Xpiks/1.1")
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
                .arg(QSysInfo::productVersion());
#else
                .arg("?");
#endif
#endif
        std::string userAgentString = userAgent.toStdString();
        const char *userAgentData = userAgentString.data();
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, userAgentData);

        std::string postString = payload.toStdString();
        const char *postData = postString.data();
        /* Now specify the POST data */
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postData);

        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, (long)postString.size());

        /* get it! */
        res = curl_easy_perform(curl_handle);

        const bool success = (CURLE_OK == res);

        /* check for errors */
        if(!success) {
            LOG_WARNING << "curl_easy_perform() failed" << curl_easy_strerror(res);
        }

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        return success;
    }
}
