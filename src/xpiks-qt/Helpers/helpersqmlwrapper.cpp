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

#include "helpersqmlwrapper.h"
#include <QStringList>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include "keywordvalidator.h"
#include "../Commands/commandmanager.h"
#include "logger.h"

namespace Helpers {
    HelpersQmlWrapper::HelpersQmlWrapper(Commands::CommandManager *commandManager) {
        m_CommandManager = commandManager;
    }

    bool HelpersQmlWrapper::isKeywordValid(const QString &keyword) const {
        return isValidKeyword(keyword);
    }

    QString HelpersQmlWrapper::sanitizeKeyword(const QString &keyword) const {
        return doSanitizeKeyword(keyword);
    }

    void HelpersQmlWrapper::afterConstruction() {
        m_CommandManager->afterConstructionCallback();
    }

    void HelpersQmlWrapper::beforeDestruction() {
        m_CommandManager->beforeDestructionCallback();
    }

    void HelpersQmlWrapper::revealLogFile() {
        QString logFilePath = Logger::getInstance().getLogFilePath();
#ifdef Q_OS_MAC
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\"";
        args << "-e";
        args << "activate";
        args << "-e";
        args << "select POSIX file \"" + logFilePath + "\"";
        args << "-e";
        args << "end tell";
        QProcess::startDetached("osascript", args);
#endif

#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(logFilePath);
    QProcess::startDetached("explorer", args);
#endif
    }

    void Helpers::HelpersQmlWrapper::reportOpen() {
        m_CommandManager->reportUserAction(Conectivity::UserActionOpen);
    }

    void Helpers::HelpersQmlWrapper::cleanupLocalLibrary() const {
        m_CommandManager->cleanupLocalLibraryAsync();
    }

}


