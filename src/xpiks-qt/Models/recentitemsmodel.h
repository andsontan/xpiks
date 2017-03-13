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

#ifndef RECENTITEMSMODEL_H
#define RECENTITEMSMODEL_H

#include <QAbstractListModel>
#include <QQueue>
#include <QString>
#include <QSet>
#include <QHash>
#include <QUrl>

namespace Models {
    class RecentItemsModel: public QAbstractListModel
    {
        Q_OBJECT
    public:
        RecentItemsModel();
        RecentItemsModel(int items): m_MaxRecentItems(items) {}

    public:
        Q_INVOKABLE QString serializeForSettings();
        void deserializeFromSettings(const QString &serialized);
        void pushItem(const QString &directoryPath);
        Q_INVOKABLE QUrl getLatestItem() const;
        int getMaxRecentItems() const { return m_MaxRecentItems; }

#ifdef CORE_TESTS
        QString getLatestUsedItem() const { return m_LatestUsedItem; }
#endif

    private:
        bool doPushItem(const QString &itemPath);

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const override { Q_UNUSED(parent); return m_RecentItems.length(); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    protected:
        virtual QHash<int, QByteArray> roleNames() const override { return QAbstractListModel::roleNames(); }

    private:
        QSet<QString> m_ItemsSet;
        QQueue<QString> m_RecentItems;
        int m_MaxRecentItems;
        QString m_LatestUsedItem;
    };
}

#endif // RECENTITEMSMODEL_H
