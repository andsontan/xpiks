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
#include <QString>
#include "../Helpers/stringhelper.h"
#include "../Common/defines.h"

namespace AutoComplete {
    StringFilterProxyModel::StringFilterProxyModel():
        m_Threshold(2),
        m_SelectedIndex(-1),
        m_IsActive(false)
    {
        setSourceModel(&m_StringsModel);
    }

    void StringFilterProxyModel::setSearchTerm(const QString &value) {
        if (value != m_SearchTerm) {
            m_SearchTerm = value;
            emit searchTermChanged(value);
        }

        invalidateFilter();
    }

    void StringFilterProxyModel::setStrings(const QStringList &list) {
        LOG_INFO << "Adding" << list.length() << "values";
        m_StringsList = list;
        m_StringsModel.setStringList(m_StringsList);
    }

    bool StringFilterProxyModel::moveSelectionUp() {
        bool canMove = m_SelectedIndex > 0;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex - 1);
        }
        return canMove;
    }

    bool StringFilterProxyModel::moveSelectionDown() {
        bool canMove = m_SelectedIndex < rowCount() - 1;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex + 1);
        }
        return canMove;
    }

    void StringFilterProxyModel::acceptSelected() {
        LOG_DEBUG << "Selected index:" << m_SelectedIndex;

        if (0 <= m_SelectedIndex && m_SelectedIndex < rowCount()) {
            QModelIndex proxyIndex = this->index(m_SelectedIndex, 0);
            QModelIndex originalIndex = this->mapToSource(proxyIndex);
            int index = originalIndex.row();

            LOG_DEBUG << "Real index:" << index;

            emit completionAccepted(m_StringsList.at(index));
        }

        emit dismissPopupRequested();

        setIsActive(false);
    }

    bool StringFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
        Q_UNUSED(sourceParent);
        if (sourceRow < 0 || sourceRow >= m_StringsList.length()) { return false; }
        if (m_SearchTerm.trimmed().isEmpty()) { return true; }

        QString item = m_StringsList.at(sourceRow).toLower();
        if (item.contains(m_SearchTerm, Qt::CaseInsensitive)) { return true; }

        int distance = Helpers::levensteinDistance(item.left(m_SearchTerm.length() + m_Threshold - 1), m_SearchTerm);
        return distance <= m_Threshold;
    }
}
