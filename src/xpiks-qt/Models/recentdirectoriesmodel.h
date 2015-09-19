/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef RECENTDIRECTORIESMODEL_H
#define RECENTDIRECTORIESMODEL_H

#include <QAbstractListModel>
#include <QQueue>
#include <QString>
#include <QSet>
#include <QHash>

namespace Models {
    class RecentDirectoriesModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        RecentDirectoriesModel();

    public:
        Q_INVOKABLE QString serializeForSettings();
        void deserializeFromSettings(const QString &serialized);
        void pushDirectory(const QString &directoryPath);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_RecentDirectories.length(); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        virtual QHash<int, QByteArray> roleNames() const { return QAbstractListModel::roleNames(); }

    private:
        QSet<QString> m_DirectoriesSet;
        QQueue<QString> m_RecentDirectories;
        int m_MaxRecentDirectories;
    };
}

#endif // RECENTDIRECTORIESMODEL_H
