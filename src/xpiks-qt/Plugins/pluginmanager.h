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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QVector>
#include <QHash>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "../Common/baseentity.h"
#include "uiprovider.h"

namespace Plugins {
    class XpiksPluginInterface;
    class PluginWrapper;

    class PluginManager : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        PluginManager();
        virtual ~PluginManager();

    public:
        enum UploadInfoRepository_Roles {
            PrettyNameRole = Qt::UserRole + 1,
            VersionRole,
            AuthorRole,
            PluginIDRole,
            IsEnabledRole
        };

    public:
        void loadPlugins();
        void unloadPlugins();
        bool hasExportedActions(int row) const;
        UIProvider *getUIProvider() { return &m_UIProvider; }

    public:
        Q_INVOKABLE QObject *getPluginActions(int index) const;
        Q_INVOKABLE void triggerPluginAction(int pluginID, int actionID) const;

    private:
        void addPlugin(XpiksPluginInterface *plugin);

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        int getNextPluginID() { return m_LastPluginID++; }

    private:
        QVector<PluginWrapper *> m_PluginsList;
        QHash<int, PluginWrapper *> m_PluginsDict;
        UIProvider m_UIProvider;
        int m_LastPluginID;
    };

    class PluginsWithActionsModel: public QSortFilterProxyModel {
        Q_OBJECT
    public:
        PluginsWithActionsModel(QObject *parent = 0):
            QSortFilterProxyModel(parent)
        {}
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    };
}

#endif // PLUGINMANAGER_H
