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

#ifndef AUTOCOMPLETEMODEL_H
#define AUTOCOMPLETEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>
#include "../Common/baseentity.h"

namespace AutoComplete {
    class AutoCompleteModel : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int selectedIndex READ getSelectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)
        Q_PROPERTY(bool isActive READ getIsActive WRITE setIsActive NOTIFY isActiveChanged)
    public:
        AutoCompleteModel(QObject *parent=0);

    public:
        void setCompletions(const QStringList &completions);

    public:
        int getSelectedIndex() const { return m_SelectedIndex; }
        bool getIsActive() const { return m_IsActive; }

    public:
        void setSelectedIndex(int value) {
            if (value != m_SelectedIndex) {
                m_SelectedIndex = value;
                emit selectedIndexChanged(value);
            }
        }

        void setIsActive(bool value) {
            if (value != m_IsActive) {
                m_IsActive = value;
                emit isActiveChanged(value);
            }
        }

    public:
        Q_INVOKABLE bool moveSelectionUp();
        Q_INVOKABLE bool moveSelectionDown();
        Q_INVOKABLE void cancelCompletion() { setIsActive(false); emit dismissPopupRequested(); }
        Q_INVOKABLE void acceptSelected();
        Q_INVOKABLE int getCount() const { return qMax(m_CompletionList.length(), m_LastGeneratedCompletions.length()); }
        Q_INVOKABLE bool hasSelectedCompletion() { return (0 <= m_SelectedIndex) && (m_SelectedIndex < m_CompletionList.length()); }

    signals:
        void selectedIndexChanged(int index);
        void dismissPopupRequested();
        void completionAccepted(const QString &completion);
        void isActiveChanged(bool value);

    public slots:
        void completionsArrived();

        // QAbstractItemModel interface
    public:
        virtual int rowCount(const QModelIndex &parent) const { Q_UNUSED(parent); return m_CompletionList.length(); }
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual QHash<int, QByteArray> roleNames() const;

#ifdef INTEGRATION_TESTS
        bool containsWord(const QString &word) { return m_CompletionList.contains(word); }
#endif

    private:
        QVector<QString> m_CompletionList;
        QStringList m_LastGeneratedCompletions;
        int m_SelectedIndex;
        bool m_IsActive;
    };
}

#endif // AUTOCOMPLETEMODEL_H
