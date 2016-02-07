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
#include "../Common/baseentity.h"

namespace Plugins {
    class XpiksPluginInterface;

    class PluginManager : public Common::BaseEntity, public QAbstractListModel
    {
    public:
        PluginManager();

    public:
        enum UploadInfoRepositoryRoles {
            PrettyNameRole = Qt::UserRole + 1,
            VersionRole,
            AuthorRole
        };

    public:
        void loadPlugins();

    private:
        void addPlugin(XpiksPluginInterface *plugin);

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const;
        virtual QVariant data(const QModelIndex &index, int role) const;
    protected:
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        QVector<XpiksPluginInterface *> m_PluginsList;
    };
}

#endif // PLUGINMANAGER_H
