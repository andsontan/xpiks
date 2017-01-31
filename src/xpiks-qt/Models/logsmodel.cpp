/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

#include "logsmodel.h"
#include "../Helpers/loggingworker.h"
#include <QThread>
#include <QString>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include "../Helpers/stringhelper.h"
#include "../Helpers/logger.h"
#include "../Helpers/loghighlighter.h"
#include "../Common/defines.h"

namespace Models {

    LogsModel::LogsModel(QMLExtensions::ColorsModel *colorsModel, QObject *parent) :
        QObject(parent),
        m_LoggingWorker(new Helpers::LoggingWorker()),
        m_ColorsModel(colorsModel)
    {
#ifdef WITH_LOGS
        m_WithLogs = true;
#else
        m_WithLogs = false;
#endif
    }

    LogsModel::~LogsModel() {
    }

    void LogsModel::startLogging() {
        QThread *loggingThread = new QThread();
        m_LoggingWorker->moveToThread(loggingThread);

        QObject::connect(loggingThread, SIGNAL(started()), m_LoggingWorker, SLOT(process()));
        QObject::connect(m_LoggingWorker, SIGNAL(stopped()), loggingThread, SLOT(quit()));

        QObject::connect(m_LoggingWorker, SIGNAL(stopped()), m_LoggingWorker, SLOT(deleteLater()));
        QObject::connect(loggingThread, SIGNAL(finished()), loggingThread, SLOT(deleteLater()));

        loggingThread->start();
    }

    void LogsModel::stopLogging() {
        m_LoggingWorker->cancel();
    }

    QString LogsModel::getAllLogsText(bool moreLogs) {
        QString result;
#ifdef WITH_LOGS
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        QString logFilePath = logger.getLogFilePath();
        QFile f(logFilePath);

        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // 1000 - do not load the UI
            // advanced users will open logs it notepad
            int numberOfLines = moreLogs ? 1000 : 100;
            QString text = QString::fromUtf8(f.readAll());
            result = Helpers::getLastNLines(text, numberOfLines);
            f.close();
        }
#else
        Q_UNUSED(moreLogs);
        result = QString::fromLatin1("Logs are not available in this version");
#endif
        return result;

    }

    void LogsModel::initLogHighlighting(QQuickTextDocument *document) {
        Helpers::LogHighlighter *highlighter = new Helpers::LogHighlighter(m_ColorsModel, document->textDocument());
        Q_UNUSED(highlighter);
    }
}
