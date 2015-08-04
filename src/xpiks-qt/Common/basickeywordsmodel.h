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

#ifndef BASICKEYWORDSMODEL_H
#define BASICKEYWORDSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QVariant>
#include <QByteArray>
#include <QHash>
#include "baseentity.h"

namespace Common {

    class BasicKeywordsModel : public QAbstractListModel, public Common::BaseEntity {
        Q_OBJECT
    public:
        BasicKeywordsModel(QObject *parent) :
            QAbstractListModel(parent)
        { }

    public:
        enum BasicKeywordsModellRoles {
            KeywordRole = Qt::UserRole + 1
        };

    public:
        int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_KeywordsList.length(); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const {
            if (index.row() < 0 || index.row() >= m_KeywordsList.count()) return QVariant();
            if (role == KeywordRole) { return m_KeywordsList[index.row()]; }
            return QVariant();
        }
        const QStringList &getKeywords() const { return m_KeywordsList; }
        void reset(const QStringList &items) { beginResetModel(); m_KeywordsList.clear(); m_KeywordsList.append(items); endResetModel(); }
        void clear() { beginResetModel(); m_KeywordsList.clear(); endResetModel(); }
        void appendKeyword(const QString &keyword) { beginInsertRows(QModelIndex(), rowCount(), rowCount());
                                                     m_KeywordsList.append(keyword); endInsertRows(); }
        bool removeKeyword(int index, QString &keyword) { bool indexValid = index >= 0 && index < m_KeywordsList.length(); if (indexValid) {
                beginRemoveRows(QModelIndex(), index, index); keyword = m_KeywordsList.takeAt(index); endRemoveRows(); }
            return indexValid;
                                                        }
        bool removeLastKeyword(QString &keyword) { return removeKeyword(m_KeywordsList.length() - 1, keyword); }

    protected:
        QHash<int, QByteArray> roleNames() const {
            QHash<int, QByteArray> roles; roles[KeywordRole] = "keyword"; return roles;
        }

    private:
        QStringList m_KeywordsList;
    };
}

Q_DECLARE_METATYPE(Common::BasicKeywordsModel*)

#endif // BASICKEYWORDSMODEL_H
