/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef LOGSMODEL
#define LOGSMODEL

#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include "../Helpers/constants.h"
#include "../Helpers/stringhelper.h"
#include "../Helpers/logger.h"

namespace Models {
    class LogsModel : public QObject {
        Q_OBJECT
    public:
        void setLogsManager(Helpers::LogsManager *logsManager) { m_LogsManager = logsManager; }

        Q_INVOKABLE QString getAllLogsText(bool reallyAll=false) {
            QString result;
#ifdef QT_NO_DEBUG
            QDir logFileDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
            QString logFilePath = logFileDir.filePath(Constants::LOG_FILENAME);
            QFile f(logFilePath);

            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                // 1000 - do not load the UI
                // advanced users will open logs it notepad
                int numberOfLines = reallyAll ? 1000 : 100;
                QString text = f.readAll();
                result = Helpers::getLastNLines(text, numberOfLines);
                f.close();
            }
#else
            Q_UNUSED(reallyAll);
            result = QString::fromLatin1("Logs are available in Release version");
#endif
            return result;

        }

        Q_INVOKABLE void clearLogs() {
#ifdef QT_NO_DEBUG
            Q_ASSERT(m_LogsManager != NULL);
            m_LogsManager->clearLogs();
#endif
        }

        Q_INVOKABLE void startFlushing() {
#ifdef QT_NO_DEBUG
            Q_ASSERT(m_LogsManager != NULL);
            m_LogsManager->startFlushing();
#endif
        }

    private:
        Helpers::LogsManager *m_LogsManager;
    };
}

#endif // LOGSMODEL

