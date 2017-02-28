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

#include "tabsmodel.h"
#include "../Common/defines.h"

namespace QMLExtensions {
    bool compareCachePairs(const std::pair<int, int> &left, const std::pair<int, int> &right) {
        // first item is cache tag
        return left.first < right.first;
    }

    TabsModel::TabsModel(QObject *parent) :
        QAbstractListModel(parent)
    {
    }

    int TabsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);
        return m_TabsList.size();
    }

    QVariant TabsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if ((row < 0) || (row >= m_TabsList.size())) {
            return QVariant();
        }

        auto &tab = m_TabsList[row];

        switch (role) {
        case TabIconPathRole: return tab.m_TabIconPath;
        case TabComponentPathRole: return tab.m_TabComponentPath;
        default: return QVariant();
        }
    }

    QHash<int, QByteArray> TabsModel::roleNames() const {
        auto roles = QAbstractListModel::roleNames();
        roles[TabIconPathRole] = "tabicon";
        roles[TabComponentPathRole] = "tabcomponent";
        return roles;
    }

    void TabsModel::addSystemTab(const QString &iconPath, const QString &componentPath) {
        LOG_INFO << iconPath << componentPath;
        addTab(iconPath, componentPath);
    }

    void TabsModel::addPluginTab(const QString &iconPath, const QString &componentPath) {
        LOG_INFO << iconPath << componentPath;
        addTab(iconPath, componentPath);
    }

    void TabsModel::touchTab(int index) {
        LOG_INFO << index;

        if ((0 <= index) && (index < m_TabsList.size())) {
            auto &tab = m_TabsList[index];
            tab.m_CacheTag++;

            m_LRUcache.emplace_back(tab.m_CacheTag, index);
            std::push_heap(m_LRUcache.begin(), m_LRUcache.end(), compareCachePairs);

            if (m_LRUcache.size() > 10*m_TabsList.size()) {
                rebuildCache();
            }
        }
    }

    void TabsModel::addTab(const QString &iconPath, const QString &componentPath) {
        int lastIndex = m_TabsList.length();
        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        {
            m_TabsList.push_back(TabModel());
            auto &item = m_TabsList[lastIndex];
            item.m_TabIconPath = iconPath;
            item.m_TabComponentPath = componentPath;
            item.m_CacheTag = 0;
        }
        endInsertRows();
    }

    void TabsModel::rebuildCache() {
        LOG_DEBUG << "#";

        const int size = m_TabsList.size();

        m_LRUcache.clear();
        m_LRUcache.reserve(size);

        for (int i = 0; i < size; ++i) {
            auto &item = m_TabsList[i];
            m_LRUcache.emplace_back(item.m_CacheTag, i);
        }

        std::make_heap(m_LRUcache.begin(), m_LRUcache.end(), compareCachePairs);
    }
}
