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

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

namespace Models {
    class UIManager : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QStringList tabsList READ getTabsList NOTIFY tabsListChanged)
        Q_PROPERTY(QStringList tabsIcons READ getTabsIcons NOTIFY tabsIconsChanged)
    public:
        explicit UIManager(QObject *parent = 0);

    public:
        QStringList getTabsList() const { return m_TabsList; }
        QStringList getTabsIcons() const { return m_TabsIconsList; }

    public:
        void addTab(const QString tabIconComponent, const QString &tabComponent);
        void updateTabs();

    signals:
        void tabsListChanged();
        void tabsIconsChanged();

    private:
        QStringList m_TabsList;
        QStringList m_TabsIconsList;
    };
}

#endif // UIMANAGER_H
