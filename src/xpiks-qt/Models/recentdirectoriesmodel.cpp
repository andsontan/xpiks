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

#include "recentdirectoriesmodel.h"
#include <QDataStream>
#include <QUrl>
#include "../Common/defines.h"

namespace Models {
    RecentDirectoriesModel::RecentDirectoriesModel():
        QAbstractListModel(),
        m_MaxRecentDirectories(5)
    {
    }

    QString RecentDirectoriesModel::serializeForSettings() {
        QByteArray raw;
        QDataStream ds(&raw, QIODevice::WriteOnly);
        ds << m_RecentDirectories;
        return QString::fromLatin1(raw.toBase64());
    }

    void RecentDirectoriesModel::deserializeFromSettings(const QString &serialized) {
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

        m_DirectoriesSet = toBeAdded;
        m_RecentDirectories = deserialized;
    }

    void RecentDirectoriesModel::pushDirectory(const QString &directoryPath) {
        if (doPushDirectory(directoryPath)) {
            LOG_DEBUG << "Added new recent directory";
        }

        m_LatestUsedDirectory = directoryPath;
    }

    QUrl RecentDirectoriesModel::getLatestDirectory() const {
        return QUrl::fromLocalFile(m_LatestUsedDirectory);
    }

    bool RecentDirectoriesModel::doPushDirectory(const QString &directoryPath) {
        bool added = false;

        if (!m_DirectoriesSet.contains(directoryPath)) {
            m_DirectoriesSet.insert(directoryPath);

            int length = m_RecentDirectories.length();
            beginInsertRows(QModelIndex(), length, length);
            m_RecentDirectories.push_back(directoryPath);
            endInsertRows();

            if (m_RecentDirectories.length() > m_MaxRecentDirectories) {
                QString directoryToRemove = m_RecentDirectories.first();
                beginRemoveRows(QModelIndex(), 0, 0);
                m_RecentDirectories.pop_front();
                endRemoveRows();
                m_DirectoriesSet.remove(directoryToRemove);
            }

            added = true;
        }

        return added;
    }

    QVariant RecentDirectoriesModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_RecentDirectories.length()) return QVariant();
        if (role == Qt::DisplayRole) { return m_RecentDirectories.at(index.row()); }
        return QVariant();
    }
}

