
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

#include "logger.h"
#include <QStringList>
#include <QString>
#include <QMutexLocker>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

namespace Helpers {
    void Logger::flush() {
        int previousIndex = 0;

        m_Mutex.lock();
        {
            previousIndex = m_ActiveIndex;
            m_ActiveIndex = (m_ActiveIndex + 1) % 2;
        }
        m_Mutex.unlock();

        flushStream(previousIndex);
    }

    void Logger::clear() {
        m_Mutex.lock();
        {
            m_LogsStorage[0].clear();
            m_LogsStorage[1].clear();
            m_ActiveIndex = 0;
        }
        m_Mutex.unlock();

        clearStream();
    }

    void Logger::flushStream(int index) {
        QStringList &logItems = m_LogsStorage[index];

        if (logItems.length() > 0) {
            QMutexLocker locker(&m_StreamMutex);

            QFile outFile(m_LogFilepath);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
                QTextStream ts(&outFile);

                foreach (const QString &logItem, logItems) {
                    ts << logItem;
                    endl(ts);
                }

                logItems.clear();
            }
        }
    }

    void Logger::clearStream() {
        QMutexLocker locker(&m_StreamMutex);

        QFile outFile(m_LogFilepath);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream ts(&outFile);
            ts << QDateTime::currentDateTimeUtc().toString("dd.MM.yyyy hh:mm:ss.zzz") << " - cleared log";
            endl(ts);
        }
    }
}
