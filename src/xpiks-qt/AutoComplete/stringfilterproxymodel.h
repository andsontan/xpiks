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

#ifndef STRINGFILTERPROXYMODEL_H
#define STRINGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QString>

namespace AutoComplete {
    // designed to use only with QStringListModel
    class StringFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
        Q_PROPERTY(QString searchTerm READ getSearchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    public:
        StringFilterProxyModel();

    public:
        const QString &getSearchTerm() const { return m_SearchTerm; }
        void setSearchTerm(const QString &value);
        void setStrings(const QStringList &list);

    signals:
        void searchTermChanged(const QString &value);

        // QSortFilterProxyModel interface
    protected:
        virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    private:
        // ignore default regexp from proxymodel
        QString m_SearchTerm;
        int m_Threshold;
        QStringListModel m_StringsModel;
        QStringList m_StringsList;
    };
}

#endif // STRINGFILTERPROXYMODEL_H
