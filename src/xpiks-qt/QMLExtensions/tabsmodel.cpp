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
    bool compareCachePairs(const TabsModel::CachedTab &left, const TabsModel::CachedTab &right) {
        return left.m_CacheTag < right.m_CacheTag;
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
        m_TabsList.last().m_IsSystemTab = true;
    }

    void TabsModel::addPluginTab(const QString &iconPath, const QString &componentPath) {
        LOG_INFO << iconPath << componentPath;
        addTab(iconPath, componentPath);
    }

    bool TabsModel::removePluginTab(int index) {
        LOG_INFO << index;

        bool success = false;

        if ((0 <= index) && (index < m_TabsList.size())) {
            Q_ASSERT(!m_TabsList[index].m_IsSystemTab);
            if (!m_TabsList[index].m_IsSystemTab) {
                beginRemoveRows(QModelIndex(), index, index);
                {
                    m_TabsList.removeAt(index);
                }
                endRemoveRows();

                rebuildCache();
                emit tabRemoved();
                success = true;
            } else {
                LOG_WARNING << "Attempt to remove system tab";
            }
        }

        return success;
    }

    bool TabsModel::isTabActive(int index) {
        bool found = false;

        size_t i = 0;
        const size_t size = m_LRUcache.size();

        while ((i < 3) && (i < size)) {
            if (m_LRUcache[i].m_TabIndex == index) {
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

        touchTab(m_LRUcache[0].m_CacheTag);

        int indexToStay, indexToGo;
        int diff = m_LRUcache[1].m_CacheTag - m_LRUcache[2].m_CacheTag;
        if (diff < 0) { indexToGo = 1; indexToStay = 2; }
        else if (diff > 0) { indexToGo = 2; indexToStay = 1; }
        else { indexToGo = qrand()%2 + 1; indexToStay = 3 - indexToGo; }

        int tabIndexToStay = m_LRUcache[indexToStay].m_TabIndex;
        touchTab(tabIndexToStay);

        int tabIndexToGo = m_LRUcache[indexToGo].m_TabIndex;
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

    TabsModel::TabModel &TabsModel::getTab(int index) {
        Q_ASSERT((0 <= index) && (index < m_TabsList.size()));
        return m_TabsList[index];
    }

    void TabsModel::recacheTab(int index) {
        Q_ASSERT((0 <= index) && (index < m_TabsList.size()));
        auto &tab = m_TabsList[index];
        m_LRUcache.emplace_back(tab.m_CacheTag, index);

        if (m_LRUcache.size() > 10*(size_t)m_TabsList.size()) {
            rebuildCache();
        } else {
            std::push_heap(m_LRUcache.begin(), m_LRUcache.end(), compareCachePairs);
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
            item.m_IsSystemTab = false;
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
        emit cacheRebuilt();
    }

    DependentTabsModel::DependentTabsModel(QObject *parent):
        QSortFilterProxyModel(parent)
    { }

    void DependentTabsModel::openTab(int index) {
        doOpenTab(index);
    }

    void DependentTabsModel::onInvalidateRequired() {
        LOG_DEBUG << "#";
        invalidateFilter();
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

    ActiveTabsModel::ActiveTabsModel(QObject *parent):
        DependentTabsModel(parent)
    { }

    void ActiveTabsModel::onInactiveTabOpened(int index) {
        LOG_INFO << index;
        invalidateFilter();
    }

    bool ActiveTabsModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        Q_UNUSED(source_parent);
        auto *tabsModel = getTabsModel();
        bool isActiveTab = tabsModel->isTabActive(source_row);
        return isActiveTab;
    }

    void ActiveTabsModel::doOpenTab(int index) {
        LOG_INFO << index;
        int originalIndex = getOriginalIndex(index);
        auto *tabsModel = getTabsModel();

        tabsModel->touchTab(originalIndex);
    }

    InactiveTabsModel::InactiveTabsModel(QObject *parent):
        DependentTabsModel(parent)
    { }

    bool InactiveTabsModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        Q_UNUSED(source_parent);
        auto *tabsModel = getTabsModel();
        bool isActiveTab = tabsModel->isTabActive(source_row);
        return !isActiveTab;
    }

    bool InactiveTabsModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const {
        auto *tabsModel = getTabsModel();

        if (tabsModel->rowCount(QModelIndex()) <= 6) {
            return DependentTabsModel::lessThan(source_left, source_right);
        }

        auto &leftTab = tabsModel->getTab(source_left.row());
        auto &rightTab = tabsModel->getTab(source_right.row());
        bool less = leftTab.m_CacheTag < rightTab.m_CacheTag;
        return less;
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
