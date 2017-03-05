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

#ifndef TABSMODEL_H
#define TABSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QString>
#include <QVector>
#include <QSet>
#include <vector>
#include <utility>

namespace QMLExtensions {
    class TabsModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        explicit TabsModel(QObject *parent = 0);

    private:
        enum TabsModel_Roles {
            TabIconPathRole = Qt::UserRole + 1
            ,TabComponentPathRole
#ifdef QT_DEBUG
            ,CacheTagRole
#endif
        };

    public:
        struct CachedTab {
            CachedTab(unsigned int cacheTag, int tabIndex):
                m_CacheTag(cacheTag),
                m_TabIndex(tabIndex)
            { }

            unsigned int m_CacheTag;
            int m_TabIndex;
        };

    private:
        struct TabModel {
            QString m_TabIconPath;
            QString m_TabComponentPath;
            unsigned int m_CacheTag;
            bool m_IsSystemTab;
        };

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    signals:
        void tabRemoved();
        void cacheRebuilt();

    public:
        void addSystemTab(const QString &iconPath, const QString &componentPath);
        void addPluginTab(const QString &iconPath, const QString &componentPath);
        bool removePluginTab(int index);
        bool isTabActive(int index);
        void activateTab(int index);
        void escalateTab(int index);
        bool touchTab(int index);
        TabModel &getTab(int index);
        void updateCache();

    private:
        void recacheTab(int index);
        void addTab(const QString &iconPath, const QString &componentPath);
        void rebuildCache();
        void updateActiveTabs();

    private:
        QVector<TabModel> m_TabsList;
        std::vector<CachedTab> m_LRUcache;
        QSet<int> m_ActiveTabs;
    };

    class DependentTabsModel: public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        explicit DependentTabsModel(QObject *parent = 0);

    public:
        Q_INVOKABLE void openTab(int index);
        Q_INVOKABLE int getIndex(int index) { return getOriginalIndex(index); }

    public slots:
        void onInvalidateRequired();

    protected:
        virtual void doOpenTab(int index) = 0;
        TabsModel *getTabsModel() const;
        int getOriginalIndex(int index) const;
    };

    class ActiveTabsModel: public DependentTabsModel
    {
        Q_OBJECT
    public:
        explicit ActiveTabsModel(QObject *parent = 0);

    public slots:
        void onInactiveTabOpened(int index);

    signals:
        void tabActivateRequested(int originalTabIndex);

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

        // DependentTabsModel interface
    protected:
        virtual void doOpenTab(int index) override;
    };

    class InactiveTabsModel: public DependentTabsModel
    {
        Q_OBJECT
    public:
        explicit InactiveTabsModel(QObject *parent = 0);

    signals:
        void tabOpened(int index);

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

        // DependentTabsModel interface
    protected:
        virtual void doOpenTab(int index) override;
    };
}

#endif // TABSMODEL_H
