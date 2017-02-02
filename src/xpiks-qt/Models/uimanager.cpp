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

#include "uimanager.h"
#include "../Common/defines.h"

namespace Models {
    UIManager::UIManager(QObject *parent) :
        QObject(parent),
        m_TabID(0)
    {
    }

    void UIManager::addSystemTab(const QString tabIconComponent, const QString &tabComponent) {
        LOG_INFO << "icon" << tabIconComponent << "contents" << tabComponent;
        m_TabsList.append(tabComponent);
        m_TabsIconsList.append(tabIconComponent);
    }

    int UIManager::addPluginTab(int pluginID, const QString tabIconComponent, const QString &tabComponent) {
        LOG_INFO << "plugin" << pluginID << "icon" << tabIconComponent << "contents" << tabComponent;

        int index = m_TabsList.length();
        m_TabsList.append(tabComponent);
        m_TabsIconsList.append(tabIconComponent);

        int id = generateNextTabID();
        Q_ASSERT(!m_TabsIDsToIndex.contains(id));
        m_TabsIDsToIndex.insert(id, index);

        if (!m_PluginIDToTabIDs.contains(pluginID)) {
            m_PluginIDToTabIDs.insert(pluginID, QSet<int>());
        }

        m_PluginIDToTabIDs[pluginID].insert(id);

        LOG_INFO << "Added tab with ID" << id;

        return id;
    }

    bool UIManager::removePluginTab(int pluginID, int tabID) {
        LOG_INFO << "plugin" << pluginID << "tab" << tabID;
        bool success = false;

        do {
            if (!m_PluginIDToTabIDs.contains(pluginID)) {
                break;
            }

            auto &tabsSet = m_PluginIDToTabIDs[pluginID];
            if (!tabsSet.contains(tabID)) {
                break;
            }

            if (!m_TabsIDsToIndex.contains(tabID)) {
                break;
            }

            int index = m_TabsIDsToIndex.value(tabID);
            if ((index < 0) || (index >= m_TabsList.length())) {
                break;
            }

            tabsSet.remove(tabID);
            m_TabsList.removeAt(index);
            m_TabsIconsList.removeAt(index);

            LOG_INFO << "Plugin's tab" << tabID << "removed";
            success = true;
        } while(false);

        return success;
    }

    void UIManager::updateTabs() {
        emit tabsIconsChanged();
        emit tabsListChanged();
    }
}
