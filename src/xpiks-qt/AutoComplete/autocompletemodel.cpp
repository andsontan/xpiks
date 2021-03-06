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

#include "autocompletemodel.h"
#include "../Common/defines.h"
#include "../Commands/commandmanager.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"

namespace AutoComplete {
    AutoCompleteModel::AutoCompleteModel(QObject *parent):
        QAbstractListModel(parent),
        m_SelectedIndex(-1),
        m_IsActive(false)
    {
    }

    void AutoCompleteModel::setCompletions(const QStringList &completions) {
        Q_ASSERT(!completions.isEmpty());
        LOG_INFO << completions.length() << "completions";
        m_LastGeneratedCompletions = completions;
    }

    bool AutoCompleteModel::moveSelectionUp() {
        bool canMove = m_SelectedIndex > 0;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex - 1);
        }
        return canMove;
    }

    bool AutoCompleteModel::moveSelectionDown() {
        bool canMove = m_SelectedIndex < m_CompletionList.length() - 1;
        if (canMove) {
            setSelectedIndex(m_SelectedIndex + 1);
        }
        return canMove;
    }

    void AutoCompleteModel::acceptSelected(bool tryExpandPreset) {
        LOG_DEBUG << "Selected index:" << m_SelectedIndex << "expand preset" << tryExpandPreset;

        if (0 <= m_SelectedIndex && m_SelectedIndex < m_CompletionList.length()) {
            emit completionAccepted(m_CompletionList.at(m_SelectedIndex), tryExpandPreset);
        }

        emit dismissPopupRequested();

        setIsActive(false);
    }

    void AutoCompleteModel::completionsArrived() {
        LOG_DEBUG << "Updating completions...";
        beginResetModel();

        m_CompletionList.clear();
        m_CompletionList.reserve(m_LastGeneratedCompletions.length());

        foreach (const QString &item, m_LastGeneratedCompletions) {
            m_CompletionList.append(item);
        }

        endResetModel();

        setSelectedIndex(-1);
        setIsActive(true);

#ifdef INTEGRATION_TESTS
        emit completionsUpdated();
#endif
    }

    QVariant AutoCompleteModel::data(const QModelIndex &index, int role) const {
        int row = index.row();
        if (row < 0 || row >= m_CompletionList.length()) return QVariant();
        if (role == Qt::DisplayRole) { return m_CompletionList.at(index.row()); }
        else if (role == IsPresetRole) {
            auto *presetsModel = m_CommandManager->getPresetsModel();
            QString completion = m_CompletionList.at(index.row());
            int dummy;
            bool haveOnePreset = presetsModel->tryFindSinglePresetByName(completion, false, dummy);
            return haveOnePreset;
        }
        return QVariant();
    }

    QHash<int, QByteArray> AutoCompleteModel::roleNames() const {
        QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
        roles[IsPresetRole] = "ispreset";
        return roles;
    }
}
