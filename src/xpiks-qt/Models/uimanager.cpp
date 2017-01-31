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

namespace Models {
    UIManager::UIManager(QObject *parent) :
        QObject(parent)
    {
    }

    int UIManager::addTab(const QString tabIconComponent, const QString &tabComponent) {
        int index = m_TabsList.length();
        m_TabsList.append(tabComponent);
        m_TabsIconsList.append(tabIconComponent);
        return index;
    }

    bool UIManager::removeTab(int index) {
        bool result = (0 <= index) && (index < m_TabsList.length());
        if (result) {
            m_TabsList.removeAt(index);
            m_TabsIconsList.removeAt(index);
        }

        return result;
    }

    void UIManager::updateTabs() {
        emit tabsIconsChanged();
        emit tabsListChanged();
    }
}
