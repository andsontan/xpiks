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

#include "loggingworker.h"
#include "logger.h"
#include <iostream>
#include "../Common/defines.h"

namespace Helpers {
    LoggingWorker::LoggingWorker(QObject *parent) :
        QObject(parent),
        m_Cancel(false)
    {
    }

    void LoggingWorker::process() {
        Logger &logger = Logger::getInstance();
        const int sleepTimeout = 1000;

        while (!m_Cancel) {
            logger.flush();
            QThread::usleep(sleepTimeout);
        }

        emit stopped();
    }

    void LoggingWorker::cancel() {
        m_Cancel = true;

        Logger &logger = Logger::getInstance();
        logger.stop();
    }
}
