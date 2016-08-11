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

#include "helpersqmlwrapper.h"
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QQuickWindow>
#include <QSysInfo>
#include <QCoreApplication>
#include <QQmlEngine>
#include "keywordshelpers.h"
#include "../Commands/commandmanager.h"
#include "../Models/logsmodel.h"
#include "../Models/artworkuploader.h"
#include "../AutoComplete/stringfilterproxymodel.h"
#include "../Models/ziparchiver.h"
#include "../SpellCheck/spellcheckerservice.h"
#include "../Models/deletekeywordsviewmodel.h"
#include "../Models/uploadinforepository.h"
#include "../SpellCheck/spellchecksuggestionmodel.h"
#include "logger.h"
#include "../Common/defines.h"
#include "../Helpers/filenameshelpers.h"

#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

namespace Helpers {
    HelpersQmlWrapper::HelpersQmlWrapper(Commands::CommandManager *commandManager):
        m_CommandManager(commandManager)
    {
#ifdef Q_OS_WIN
        m_WinTaskbarButtonApplicable = QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7;
        if (m_WinTaskbarButtonApplicable) {
            m_TaskbarButton = new QWinTaskbarButton(this);
        } else {
            m_TaskbarButton = NULL;
        }
#endif
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
        LOG_DEBUG << "emitting signal";
        emit globalCloseRequested();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        m_CommandManager->beforeDestructionCallback();
    }

    void HelpersQmlWrapper::revealLogFile() {
        LOG_DEBUG << "#";
        QString logFilePath = Logger::getInstance().getLogFilePath();
        HelpersQmlWrapper::revealFile(logFilePath);
    }

    void HelpersQmlWrapper::revealArtworkFile(const QString &path) {
        HelpersQmlWrapper::revealFile(path);
    }

    void Helpers::HelpersQmlWrapper::reportOpen() {
        m_CommandManager->reportUserAction(Conectivity::UserActionOpen);
    }

    void Helpers::HelpersQmlWrapper::cleanupLocalLibrary() const {
        m_CommandManager->cleanupLocalLibraryAsync();
    }

    void HelpersQmlWrapper::setProgressIndicator(QQuickWindow *window) {
#ifdef Q_OS_WIN
        if (!m_WinTaskbarButtonApplicable) { return; }
        m_TaskbarButton->setWindow(window);
#else
        Q_UNUSED(window);
#endif
    }

    void HelpersQmlWrapper::turnTaskbarProgressOn() {
#ifdef Q_OS_WIN
        if (!m_WinTaskbarButtonApplicable) { return; }
        LOG_DEBUG << "Turning on taskbar button in Windows";
        QWinTaskbarProgress *progress = m_TaskbarButton->progress();
        progress->setVisible(true);
        progress->setRange(0, 100);
        progress->setValue(0);
#endif
    }

    void HelpersQmlWrapper::setTaskbarProgress(double value) {
#ifdef Q_OS_WIN
        if (!m_WinTaskbarButtonApplicable) { return; }
        LOG_DEBUG << value;
        QWinTaskbarProgress *progress = m_TaskbarButton->progress();
        progress->setValue((int)value);
#else
        Q_UNUSED(value);
#endif
    }

    void HelpersQmlWrapper::turnTaskbarProgressOff() {
#ifdef Q_OS_WIN
        if (!m_WinTaskbarButtonApplicable) { return; }
        LOG_DEBUG << "Turning off taskbar button in Windows";
        QWinTaskbarProgress *progress = m_TaskbarButton->progress();
        progress->setVisible(false);
#endif
    }

    bool HelpersQmlWrapper::getPluginsAvailable() const {
        bool result = false;
#ifdef WITH_PLUGINS
        result = true;
#endif
        return result;
    }

    void HelpersQmlWrapper::removeUnavailableFiles() {
        m_CommandManager->removeUnavailableFiles();
    }

    void HelpersQmlWrapper::autoCompleteKeyword(const QString &keyword, QObject *notifyObject) {
#ifndef CORE_TESTS
        m_CommandManager->autoCompleteKeyword(keyword, notifyObject);
#else
        Q_UNUSED(keyword);
        Q_UNUSED(notifyObject);
#endif
    }

    bool HelpersQmlWrapper::isVector(const QString &path) const {
        return path.endsWith("eps", Qt::CaseInsensitive) ||
                path.endsWith("ai", Qt::CaseInsensitive);
    }

    QString HelpersQmlWrapper::toImagePath(const QString &path) const {
        return Helpers::getImagePath(path);
    }

    QObject *HelpersQmlWrapper::getLogsModel() {
        Models::LogsModel *model = m_CommandManager->getLogsModel();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getFtpACList() {
        auto *artworkUploader = m_CommandManager->getArtworkUploader();
        AutoComplete::StringFilterProxyModel *model = artworkUploader->getStocksCompletionSource();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getArtworkUploader() {
        auto *model = m_CommandManager->getArtworkUploader();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getZipArchiver() {
        auto *model = m_CommandManager->getZipArchiver();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getSpellCheckerService() {
        auto *service = m_CommandManager->getSpellCheckerService();
        QQmlEngine::setObjectOwnership(service, QQmlEngine::CppOwnership);
        return service;
    }

    QObject *HelpersQmlWrapper::getDeleteKeywordsModel() {
        auto *model = m_CommandManager->getDeleteKeywordsModel();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getUploadInfos() {
        auto *model = m_CommandManager->getUploadInfoRepository();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }

    QObject *HelpersQmlWrapper::getSpellCheckSuggestionsModel() {
        auto *model = m_CommandManager->getSpellSuggestionsModel();
        QQmlEngine::setObjectOwnership(model, QQmlEngine::CppOwnership);
        return model;
    }
    void HelpersQmlWrapper::revealFile(const QString &path) {
#ifdef Q_OS_MAC
        QStringList args;
        args << "-e";
        args << "tell application \"Finder\"";
        args << "-e";
        args << "activate";
        args << "-e";
        args << "select POSIX file \"" + path + "\"";
        args << "-e";
        args << "end tell";
        QProcess::startDetached("osascript", args);
#endif

#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(path);
    QProcess::startDetached("explorer", args);
#endif
    }
}


