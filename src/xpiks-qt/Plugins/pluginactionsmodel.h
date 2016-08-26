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

#ifndef PLUGINACTIONSMODEL_H
#define PLUGINACTIONSMODEL_H

#include <QAbstractListModel>
#include <QVector>

namespace Plugins {
    class IPluginAction;

    class PluginActionsModel: public QAbstractListModel
    {
        Q_OBJECT
    public:
        PluginActionsModel(const QVector<IPluginAction *> &actions, int pluginID, QObject *parent=0);

    public:
        enum PluginActionsModel_Roles {
            ActionNameRole = Qt::UserRole + 1,
            ActionCodeRole,
            PluginIDRole
        };

    public:
        int size() const { return m_PluginActions.length(); }

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QVector<IPluginAction *> m_PluginActions;
        int m_PluginID;
    };
}

#endif // PLUGINACTIONSMODEL_H
