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

#ifndef COMBOBOXTYPES
#define COMBOBOXTYPES

#include <QAbstractListModel>
#include "../common/basiccomboboxmodel.h"

namespace Models {
    enum KeywordsSize {
        NormalFontSize,
        LargeFontSize
    };

    class KeywordsSizeComboboxModel : public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(int selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

    public:
        KeywordsSizeComboboxModel() : QAbstractListModel()
        {}

    public:
        int getSelectedIndex() const { return m_ComboboxModel.getSelectedIndex(); }
        void setSelectedIndex(int value) { if (m_ComboboxModel.setSelectedIndex(value)) { emit selectedIndexChanged(); } }
        void reset() { beginResetModel(); m_ComboboxModel.reset(); endResetModel(); }
        int rowCount(const QModelIndex & parent = QModelIndex()) const { return m_ComboboxModel.rowCount(parent); }
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const { return m_ComboboxModel.data(index, role); }
        void setSelectedValue(const KeywordsSize &value) { if (m_ComboboxModel.setSelectedValue(value)) { emit selectedIndexChanged(); } }
        void addComboboxValue(KeywordsSize value, const QString &description) { m_ComboboxModel.addComboboxValue(value, description); }

    protected:
        virtual QHash<int, QByteArray> roleNames() const { return QAbstractListModel::roleNames(); }

    signals:
        void selectedIndexChanged();

    private:
        Common::BasicComboboxModel<KeywordsSize> m_ComboboxModel;
    };
}

#endif // COMBOBOXTYPES

