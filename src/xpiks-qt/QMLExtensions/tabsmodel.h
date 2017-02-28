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
#include <QString>
#include <QVector>
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
            TabIconPathRole = Qt::UserRole + 1,
            TabComponentPathRole
        };

    private:
        struct TabModel {
            QString m_TabIconPath;
            QString m_TabComponentPath;
            int m_CacheTag;
        };

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const override;
        virtual QVariant data(const QModelIndex &index, int role) const override;
        virtual QHash<int, QByteArray> roleNames() const override;

    public:
        void addSystemTab(const QString &iconPath, const QString &componentPath);
        void addPluginTab(const QString &iconPath, const QString &componentPath);
        void touchTab(int index);

    private:
        void addTab(const QString &iconPath, const QString &componentPath);
        void rebuildCache();

    private:
        QVector<TabModel> m_TabsList;
        std::vector<std::pair<int, int> > m_LRUcache;
    };
}

#endif // TABSMODEL_H
