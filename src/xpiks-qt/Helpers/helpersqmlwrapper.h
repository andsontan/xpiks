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

#ifndef HELPERSQMLWRAPPER_H
#define HELPERSQMLWRAPPER_H

#include <QObject>
#include <QString>
#include "../Common/baseentity.h"

namespace Commands {
    class CommandManager;
}

class QQuickWindow;
#ifdef Q_OS_WIN
class QWinTaskbarButton;
#endif

namespace Helpers {
    class HelpersQmlWrapper : public QObject, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(bool pluginsAvailable READ getPluginsAvailable CONSTANT)
    public:
        HelpersQmlWrapper();

    public:
        Q_INVOKABLE bool isKeywordValid(const QString &keyword) const;
        Q_INVOKABLE QString sanitizeKeyword(const QString &keyword) const;
        Q_INVOKABLE void afterConstruction();
        Q_INVOKABLE void beforeDestruction();
        Q_INVOKABLE void revealLogFile();
        Q_INVOKABLE void reportOpen();
        Q_INVOKABLE void cleanupLocalLibrary() const;
        Q_INVOKABLE void setProgressIndicator(QQuickWindow *window);
        Q_INVOKABLE void turnTaskbarProgressOn();
        Q_INVOKABLE void setTaskbarProgress(double value);
        Q_INVOKABLE void turnTaskbarProgressOff();
        Q_INVOKABLE void removeUnavailableFiles();
        Q_INVOKABLE void autoCompleteKeyword(const QString &keyword, QObject *notifyObject);
        Q_INVOKABLE void revealArtworkFile(const QString &path);
        Q_INVOKABLE bool isVector(const QString &path) const;
        Q_INVOKABLE QString toImagePath(const QString &path) const;

    public:
        void requestCloseApplication() { emit globalCloseRequested(); }

    public:
        Q_INVOKABLE QObject *getLogsModel();
        Q_INVOKABLE QObject *getFtpACList();
        Q_INVOKABLE QObject *getArtworkUploader();
        Q_INVOKABLE QObject *getZipArchiver();
        Q_INVOKABLE QObject *getSpellCheckerService();
        Q_INVOKABLE QObject *getDeleteKeywordsModel();
        Q_INVOKABLE QObject *getUploadInfos();
        Q_INVOKABLE QObject *getSpellCheckSuggestionsModel();

    public:
        bool getPluginsAvailable() const;

   private:
        void revealFile(const QString &path);

    signals:
        void globalCloseRequested();
        void globalBeforeDestruction();
        void updateAvailable(QString updateLink);

    private:
#ifdef Q_OS_WIN
        QWinTaskbarButton *m_TaskbarButton;
        bool m_WinTaskbarButtonApplicable;
#endif
    };
}

#endif // HELPERSQMLWRAPPER_H
