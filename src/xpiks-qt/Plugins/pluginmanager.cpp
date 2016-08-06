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
#include <QQmlEngine>
#include "xpiksplugininterface.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/undoredomanager.h"
#include "pluginwrapper.h"
#include "../Models/artitemsmodel.h"
#include "../Common/defines.h"

namespace Plugins {
    PluginManager::PluginManager():
        QAbstractListModel(),
        m_LastPluginID(0)
    {
    }

    PluginManager::~PluginManager() {
        qDeleteAll(m_PluginsList);
    }

    void PluginManager::loadPlugins() {
        LOG_DEBUG << "#";

        QDir pluginsDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == "MacOS") {
            pluginsDir.cdUp();
        }

        pluginsDir.cd("PlugIns");
#endif

        bool pluginsFound = pluginsDir.cd("XpiksPlugins");

        if (!pluginsFound) {
            LOG_WARNING << "Plugins directory not found";
            return;
        }

        LOG_INFO<< "Plugins dir:" << pluginsDir.absolutePath();

        beginResetModel();
        m_PluginsList.clear();

        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            LOG_DEBUG << "Trying file:" << fileName;

            QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
            QObject *plugin = loader.instance();
            if (plugin) {
                XpiksPluginInterface *xpiksPlugin = qobject_cast<XpiksPluginInterface *>(plugin);

                if (xpiksPlugin) {
                    addPlugin(xpiksPlugin);
                } else {
                    LOG_DEBUG << "Not Xpiks Plugin";
                }
            } else {
                LOG_DEBUG << loader.errorString();
            }
        }

        endResetModel();
    }

    void PluginManager::unloadPlugins() {
        int size = m_PluginsList.length();
        LOG_DEBUG << size << "plugin(s)";

        for (int i = 0; i < size; ++i) {
            PluginWrapper *wrapper = m_PluginsList.at(i);
            wrapper->finalizePlugin();
        }
    }

    bool PluginManager::hasExportedActions(int row) const {
        bool hasActions = false;

        if (0 <= row && row < m_PluginsList.length()) {
            hasActions = m_PluginsList.at(row)->anyActionsProvided();
        }

        return hasActions;
    }

    QObject *PluginManager::getPluginActions(int index) const {
        LOG_DEBUG << index;
        PluginActionsModel *item = NULL;

        if (0 <= index && index < m_PluginsList.length()) {
            item = m_PluginsList.at(index)->getActionsModel();
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        return item;
    }

    void PluginManager::triggerPluginAction(int pluginID, int actionID) const {
        LOG_INFO << "Plugin ID" << pluginID << "action ID" << actionID;
        PluginWrapper *pluginWrapper = m_PluginsDict.value(pluginID, NULL);
        if (pluginWrapper != NULL) {
            pluginWrapper->triggerActionSafe(actionID);
        }
    }

    void PluginManager::addPlugin(XpiksPluginInterface *plugin) {
        int pluginID = getNextPluginID();
        LOG_INFO << "ID:" << pluginID << "name:" << plugin->getPrettyName() << "version:" << plugin->getVersionString();

        PluginWrapper *pluginWrapper = NULL;

        try {
            plugin->injectCommandManager(m_CommandManager);
            plugin->injectUndoRedoManager(m_CommandManager->getUndoRedoManager());
            plugin->injectUIProvider(&m_UIProvider);
            plugin->injectArtworksSource(m_CommandManager->getArtItemsModel());

            plugin->initializePlugin();

            pluginWrapper = new PluginWrapper(plugin, pluginID);
        }
        catch(...) {
            LOG_WARNING << "Fail initializing plugin with ID:" << pluginID;
            pluginWrapper = NULL;
        }

        if (pluginWrapper != NULL) {
            m_PluginsList.append(pluginWrapper);
            m_PluginsDict.insert(pluginID, pluginWrapper);
        }
    }

    int PluginManager::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_PluginsList.length();
    }

    QVariant PluginManager::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_PluginsList.length()) { return QVariant(); }

        PluginWrapper *plugin = m_PluginsList.at(row);

        switch (role) {
        case PrettyNameRole:
            return plugin->getPrettyName();
        case AuthorRole:
            return plugin->getAuthor();
        case VersionRole:
            return plugin->getVersionString();
        case PluginIDRole:
            return plugin->getPluginID();
        case IsEnabledRole:
            return plugin->getIsEnabled();
        default:
            return QVariant();
        }
    }

    QHash<int, QByteArray> PluginManager::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[PrettyNameRole] = "prettyname";
        roles[AuthorRole] = "author";
        roles[VersionRole] = "version";
        roles[PluginIDRole] = "pluginID";
        roles[IsEnabledRole] = "enabled";
        return roles;
    }

    bool PluginsWithActionsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);

        QAbstractItemModel *sourceItemModel = sourceModel();
        PluginManager *pluginManager = dynamic_cast<PluginManager *>(sourceItemModel);
        Q_ASSERT(pluginManager != NULL);
        bool result = pluginManager->hasExportedActions(sourceRow);
        return result;
    }

}
