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

#include "recentitemsmodel.h"
#include <QDataStream>
#include <QUrl>
#include "../Common/defines.h"

namespace Models {
    RecentItemsModel::RecentItemsModel():
        QAbstractListModel()
    {
    }

    QString RecentItemsModel::serializeForSettings() {
        QByteArray raw;
        QDataStream ds(&raw, QIODevice::WriteOnly);
        ds << m_RecentItems;
        return QString::fromLatin1(raw.toBase64());
    }

    void RecentItemsModel::deserializeFromSettings(const QString &serialized) {
        LOG_DEBUG << "#";

        QByteArray originalData;
        originalData.append(serialized.toLatin1());
        QByteArray serializedBA = QByteArray::fromBase64(originalData);

        QQueue<QString> items;
        QDataStream ds(&serializedBA, QIODevice::ReadOnly);
        ds >> items;

        QQueue<QString> deserialized;
        QSet<QString> toBeAdded;

        foreach (const QString &item, items) {
            if (!toBeAdded.contains(item)) {
                toBeAdded.insert(item);
                deserialized.push_back(item);
            }
        }

        m_ItemsSet = toBeAdded;
        m_RecentItems = deserialized;
    }

    void RecentItemsModel::pushItem(const QString &itemPath) {
        if (doPushItem(itemPath)) {
            LOG_DEBUG << "Added new recent item";
        }

        m_LatestUsedItem = itemPath;
    }

    QUrl RecentItemsModel::getLatestItem() const {
        return QUrl::fromLocalFile(m_LatestUsedItem);
    }

    bool RecentItemsModel::doPushItem(const QString &itemPath) {
        bool added = false;

        if (!m_ItemsSet.contains(itemPath)) {
            m_ItemsSet.insert(itemPath);

            int length = m_RecentItems.length();
            beginInsertRows(QModelIndex(), length, length);
            m_RecentItems.push_back(itemPath);
            endInsertRows();

            if (m_RecentItems.length() > m_MaxRecentItems) {
                QString itemToRemove = m_RecentItems.first();
                beginRemoveRows(QModelIndex(), 0, 0);
                m_RecentItems.pop_front();
                endRemoveRows();
                m_ItemsSet.remove(itemToRemove);
            }

            added = true;
        }

        return added;
    }

    QVariant RecentItemsModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_RecentItems.length()) return QVariant();
        if (role == Qt::DisplayRole) { return m_RecentItems.at(index.row()); }
        return QVariant();
    }
}
