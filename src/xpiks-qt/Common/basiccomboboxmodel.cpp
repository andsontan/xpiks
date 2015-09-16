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

#include "basiccomboboxmodel.h"

namespace Common {
    template<typename T>
    BasicComboboxModel<T>::BasicComboboxModel() :
        QAbstractListModel(),
        m_SelectedIndex(0)
    {
    }

    template<typename T>
    bool BasicComboboxModel<T>::addComboboxValue(const T &value, const QString &description)
    {
        bool inTheList = m_ExistingValues.contains(value);
        if (!inTheList) {
            m_ExistingValues.insert(value);
            m_ComboboxValues.append(qMakePair(value, description));
        }

        return !inTheList;
    }

    template<typename T>
    QVariant BasicComboboxModel<T>::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) { return QVariant(); }

        if (role == Qt::DisplayRole) {
            return m_ComboboxValues[index.row()].second;
        } else {
            return QVariant();
        }
    }

    template<typename T>
    void BasicComboboxModel<T>::setSelectedValue(const T &value) {
        int size = rowCount();
        for (int i = 0; i < size; ++i) {
            if (m_ComboboxValues[i].first == value) {
                setSelectedIndex(i);
                break;
            }
        }
    }
}

