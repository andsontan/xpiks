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

    bool TabsModel::isActiveTab(int index) {
        bool found = false;

        size_t i = 0;
        const size_t size = m_LRUcache.size();

        while ((i < 3) && (i < size)) {
            if (m_LRUcache[i].second == index) {
                found = true;
                break;
            }

            i++;
        }

        return found;
    }

    void TabsModel::escalateTab(int index) {
        LOG_INFO << index;
        if ((index < 0) || (index >= m_TabsList.size())) { return; }

        touchTab(m_LRUcache[0].second);

        int indexToStay, indexToGo;
        int diff = m_LRUcache[1].first - m_LRUcache[2].first;
        if (diff < 0) { indexToGo = 1; indexToStay = 2; }
        else if (diff > 0) { indexToGo = 2; indexToStay = 1; }
        else { indexToGo = qrand()%2 + 1; indexToStay = 3 - indexToGo; }

        int tabIndexToStay = m_LRUcache[indexToStay].second;
        touchTab(tabIndexToStay);

        int tabIndexToGo = m_LRUcache[indexToGo].second;
        m_TabsList[index].m_CacheTag = m_TabsList[tabIndexToGo].m_CacheTag;
        touchTab(index);
    }

    bool TabsModel::touchTab(int index) {
        bool success = false;
        LOG_INFO << index;

        if ((0 <= index) && (index < m_TabsList.size())) {
            auto &tab = m_TabsList[index];
            tab.m_CacheTag++;
            recacheTab(index);
            success = true;
        }

        return success;
    }

    void TabsModel::recacheTab(int index) {
        Q_ASSERT((0 <= index) && (index < m_TabsList.size()));
        auto &tab = m_TabsList[index];
        m_LRUcache.emplace_back(tab.m_CacheTag, index);
        std::push_heap(m_LRUcache.begin(), m_LRUcache.end(), compareCachePairs);

        if (m_LRUcache.size() > 10*(size_t)m_TabsList.size()) {
            rebuildCache();
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

    void DependentTabsModel::openTab(int index) {
        doOpenTab(index);
    }

    TabsModel *DependentTabsModel::getTabsModel() const {
        QAbstractItemModel *sourceItemModel = sourceModel();
        TabsModel *tabsModel = dynamic_cast<TabsModel *>(sourceItemModel);
        Q_ASSERT(tabsModel != nullptr);
        return tabsModel;
    }

    int DependentTabsModel::getOriginalIndex(int index) const {
        LOG_INFO << index;
        QModelIndex originalIndex = mapToSource(this->index(index, 0));
        int row = originalIndex.row();
        return row;
    }

    void ActiveTabsModel::onInactiveTabOpened(int index) {
        LOG_INFO << index;
        invalidateFilter();
    }

    bool ActiveTabsModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        Q_UNUSED(source_parent);
        auto *tabsModel = getTabsModel();
        bool isActiveTab = tabsModel->isActiveTab(source_row);
        return isActiveTab;
    }

    void ActiveTabsModel::doOpenTab(int index) {
        LOG_INFO << index;
        int originalIndex = getOriginalIndex(index);
        auto *tabsModel = getTabsModel();

        tabsModel->touchTab(originalIndex);
    }

    bool InactiveTabsModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        Q_UNUSED(source_parent);
        auto *tabsModel = getTabsModel();
        bool isActiveTab = tabsModel->isActiveTab(source_row);
        return !isActiveTab;
    }

    void InactiveTabsModel::doOpenTab(int index) {
        LOG_INFO << index;
        int originalIndex = getOriginalIndex(index);
        auto *tabsModel = getTabsModel();

        tabsModel->escalateTab(originalIndex);
        invalidateFilter();
        emit tabOpened(originalIndex);
    }
}
