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

#ifndef DEFINES
#define DEFINES

#include <QtGlobal>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QStandardPaths>

#ifdef QT_NO_DEBUG
#define WITH_LOGS
#else
// will be enabled in Xpiks 1.3 at least
// #define WITH_PLUGINS
#endif

// 4*(avg word size 10 * avg keywords size 50)
#define MAX_PASTE_SIZE 2000

#if (QT_VERSION <= QT_VERSION_CHECK(5, 4, 2))
#define qInfo qDebug
#endif

#define LOG_DEBUG qDebug()
#define LOG_INFO qInfo()

#ifdef CORE_TESTS
#define LOG_CORE_TESTS qDebug()
#else
#define LOG_CORE_TESTS if (1) {} else qDebug()
#endif

#ifdef INTEGRATION_TESTS
#define LOG_INTEGRATION_TESTS qDebug()
#else
#define LOG_INTEGRATION_TESTS if (1) {} else qDebug()
#endif

#if defined(INTEGRATION_TESTS) || defined(QT_DEBUG)
#define LOG_INTEGR_TESTS_OR_DEBUG qDebug()
#else
#define LOG_INTEGR_TESTS_OR_DEBUG if (1) {} else qDebug()
#endif

#if defined(INTEGRATION_TESTS) || defined(CORE_TESTS)
#define LOG_WARNING qInfo() << "FAKE_WARNING"
#else
#define LOG_WARNING qWarning()
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#define XPIKS_DATA_LOCATION_TYPE QStandardPaths::AppDataLocation
#define XPIKS_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
#define XPIKS_DATA_LOCATION_TYPE QStandardPaths::DataLocation
#define XPIKS_USERDATA_PATH QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif

#define PREVIEWOFFSET 25
#define PREVIEWKEYWORDSCOUNT 5

#endif // DEFINES

