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

#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QString>
#include <QWaitCondition>
#include <QMutex>

namespace Helpers {
    class Logger
    {
    public:
        static Logger& getInstance()
        {
            static Logger instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    public:
        void setLogFilePath(const QString &filepath) {
            m_LogFilepath = filepath;
        }

        QString getLogFilePath() const { return m_LogFilepath; }

        void log(const QString &message);
        void flush();
        void stop();

    private:
        void doLog(const QString &message);
        void flushStream(QStringList *logItems);

    private:
        Logger() {
            m_QueueLogTo = &m_LogsStorage[0];
            m_QueueFlushFrom = &m_LogsStorage[1];
            m_Stopped = false;
        }

        Logger(Logger const&);
        void operator=(Logger const&);

    private:
        QString m_LogFilepath;
        QStringList m_LogsStorage[2];
        QStringList *m_QueueFlushFrom;
        QStringList *m_QueueLogTo;
        QMutex m_LogMutex;
        QMutex m_FlushMutex;
        QWaitCondition m_AnyLogsToFlush;
        volatile bool m_Stopped;
    };
}

#endif // LOGGER_H
