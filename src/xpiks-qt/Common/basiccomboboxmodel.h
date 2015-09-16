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

#ifndef BASICCOMBOBOXMODEL_H
#define BASICCOMBOBOXMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QString>
#include <QList>
#include <QPair>
#include <QSet>

namespace Common {
    template<typename T>
    class BasicComboboxModel
    {
    public:
        BasicComboboxModel() :
            m_SelectedIndex(0)
        {
        }

    public:
        bool addComboboxValue(const T &value, const QString &description) {
            bool inTheList = m_ExistingValues.contains(value);
            if (!inTheList) {
                m_ExistingValues.insert(value);
                m_ComboboxValues.append(qMakePair(value, description));
            }

            return !inTheList;
        }

        const T &getSelectedValue() const { return m_ComboboxValues[m_SelectedIndex].first; }
        int rowCount(const QModelIndex & parent = QModelIndex()) const { Q_UNUSED(parent); return m_ComboboxValues.length(); }
        void reset() { m_ComboboxValues.clear(); m_ExistingValues.clear(); m_SelectedIndex = 0; }

        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const {
            if (!index.isValid()) { return QVariant(); }

            if (role == Qt::DisplayRole) {
                return m_ComboboxValues[index.row()].second;
            } else {
                return QVariant();
            }
        }

        bool setSelectedValue(const T &value) {
            bool set = false;
            int size = rowCount();
            for (int i = 0; i < size; ++i) {
                if (m_ComboboxValues[i].first == value) {
                    setSelectedIndex(i);
                    set = true;
                    break;
                }
            }
            return set;
        }

    public:
        int getSelectedIndex() const { return m_SelectedIndex; }
        bool setSelectedIndex(int value) {
            bool set = false;
            if (value != m_SelectedIndex &&
                    value >= 0 && value < m_ComboboxValues.length()) {
                m_SelectedIndex = value;
                set = true;
            }
            return set;
        }

        QHash<int, QByteArray> roleNames() const { return QHash<int, QByteArray>(); }

    private:
        QSet<T> m_ExistingValues;
        QList<QPair<T, QString> > m_ComboboxValues;
        int m_SelectedIndex;
    };
}

#endif // BASICCOMBOBOXMODEL_H
