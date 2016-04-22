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

#include "stringfilterproxymodel.h"
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include "../Helpers/stringhelper.h"

namespace AutoComplete {
    StringFilterProxyModel::StringFilterProxyModel()
    { }

    void StringFilterProxyModel::setSearchTerm(const QString &value) {
        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
    }

    void StringFilterProxyModel::setStrings(const QStringList &list) {
        m_StringsList = list;
        m_StringsModel.setStringList(m_StringsList);
    }

    bool StringFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);
        if (sourceRow < 0 || sourceRow >= m_StringsList.length()) { return false; }
        if (m_SearchTerm.trimmed().isEmpty()) { return true; }

        const QString &item = m_StringsList.at(sourceRow);
        int distance = Helpers::levensteinDistance(item, m_SearchTerm);

        return distance < m_Threshold;
    }
}
