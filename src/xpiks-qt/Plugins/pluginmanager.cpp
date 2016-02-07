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

#include "pluginmanager.h"
#include <QPluginLoader>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include "xpiksplugininterface.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/undoredomanager.h"

namespace Plugins {
    PluginManager::PluginManager():
        QAbstractListModel()
    {

    }

    void PluginManager::loadPlugins() {
        qDebug() << "PluginManager::loadPlugins #" << "Loading...";

        QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
            pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif
        pluginsDir.cd("plugins");

        beginResetModel();
        m_PluginsList.clear();

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                XpiksPluginInterface *xpiksPlugin = qobject_cast<XpiksPluginInterface *>(plugin);

                if (xpiksPlugin) {
                    addPlugin(xpiksPlugin);
                }
            }
        }

        endResetModel();
    }

    void PluginManager::addPlugin(XpiksPluginInterface *plugin) {
        qInfo() << "PluginManager::addPlugin #" << "name:" << plugin->getPrettyName() << "version:" << plugin->getVersionString();
        m_PluginsList.append(plugin);

        plugin->injectCommandManager(m_CommandManager);
        plugin->injectUndoRedoManager(m_CommandManager->getUndoRedoManager());
    }

    int PluginManager::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_PluginsList.length();
    }

    QVariant PluginManager::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_PluginsList.length()) { return QVariant(); }

        XpiksPluginInterface *plugin = m_PluginsList.at(row);

        switch (role) {
        case PrettyNameRole:
            return plugin->getPrettyName();
        case AuthorRole:
            return plugin->getAuthor();
        case VersionRole:
            return plugin->getVersionString();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> PluginManager::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PrettyNameRole] = "prettyname";
        roles[AuthorRole] = "author";
        roles[VersionRole] = "version";
        return roles;
    }
}
