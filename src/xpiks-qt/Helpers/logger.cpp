
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
#include <QElapsedTimer>
#include <iostream>
#include <string>
#include <ctime>
#include "../Common/defines.h"

#define MIN_FIRE_SIZE 20
#define LOGGING_TIMEOUT 5

namespace Helpers {
    void Logger::log(const QString &message) {
        if (!m_Stopped) {
            doLog(message);
        }
    }

    void Logger::flush() {
        if (m_Stopped) { return; }

        QMutexLocker flushLocker(&m_FlushMutex);

        while (m_QueueFlushFrom->isEmpty()) {
            QMutexLocker logLocker(&m_LogMutex);

            if (m_QueueLogTo->isEmpty()) {
                m_AnyLogsToFlush.wait(&m_LogMutex);
            } else {
                qSwap(m_QueueLogTo, m_QueueFlushFrom);
            }
        }

        Q_ASSERT(m_QueueFlushFrom->length() > 0);
        flushStream(m_QueueFlushFrom);
    }

    void Logger::stop() {
        m_Stopped = true;

        // will make waiting flush() call unblocked if any
        doLog("Stop message");

        QMutexLocker flushLocker(&m_FlushMutex);
        flushStream(m_QueueFlushFrom);

        {
            QMutexLocker logLocker(&m_LogMutex);

            if (!m_QueueLogTo->isEmpty()) {
                qSwap(m_QueueLogTo, m_QueueFlushFrom);
            }
        }

        flushStream(m_QueueFlushFrom);
    }

    void Logger::doLog(const QString &message) {
        QMutexLocker locker(&m_LogMutex);
        m_QueueLogTo->append(message);
        m_AnyLogsToFlush.wakeOne();
    }

    void Logger::flushStream(QStringList *logItems) {
        Q_ASSERT(logItems != nullptr);
        if (logItems->empty()) { return; }

#ifdef WITH_LOGS
        QFile outFile(m_LogFilepath);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream ts(&outFile);
            ts.setCodec("UTF-8");

            int size = logItems->size();
            for (int i = 0; i < size; ++i) {
                const QString &line = logItems->at(i);
                ts << line;
                endl(ts);
            }
        }
#else
        int size = logItems->size();
        for (int i = 0; i < size; ++i) {
            const QString &line = logItems->at(i);
            std::cout << line.toLocal8Bit().data() << std::endl;
        }

        std::cout.flush();
#endif
        logItems->clear();
    }
}
