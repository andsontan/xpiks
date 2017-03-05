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

#include "undoredomanager.h"
#include "../Common/defines.h"

UndoRedo::UndoRedoManager::~UndoRedoManager() { }

void UndoRedo::UndoRedoManager::recordHistoryItem(std::unique_ptr<IHistoryItem> &historyItem) {
    LOG_INFO << "History item about to be recorded:" << historyItem->getActionType();

    QMutexLocker locker(&m_Mutex);

    if (!m_HistoryStack.empty()) {
        m_HistoryStack.pop();
        Q_ASSERT(m_HistoryStack.empty());
    }

    m_HistoryStack.push(std::move(historyItem));
    emit canUndoChanged();
    emit itemRecorded();
    emit undoDescriptionChanged();
}

bool UndoRedo::UndoRedoManager::undoLastAction() {
    LOG_DEBUG << "#";
    m_Mutex.lock();

    bool anyItem = false;
    anyItem = !m_HistoryStack.empty();

    if (anyItem) {
        std::unique_ptr<UndoRedo::IHistoryItem> historyItem(std::move(m_HistoryStack.top()));
        m_HistoryStack.pop();
        m_Mutex.unlock();

        emit canUndoChanged();
        emit undoDescriptionChanged();
        int commandID = historyItem->getCommandID();
        historyItem->undo(m_CommandManager);
        emit actionUndone(commandID);
    } else {
        m_Mutex.unlock();
        LOG_WARNING << "No item for undo";
    }

    return anyItem;
}

void UndoRedo::UndoRedoManager::discardLastAction() {
    LOG_DEBUG << "#";
    m_Mutex.lock();

    bool anyItem = false;
    anyItem = !m_HistoryStack.empty();

    if (anyItem) {
        std::unique_ptr<UndoRedo::IHistoryItem> historyItem(std::move(m_HistoryStack.top()));
        m_HistoryStack.pop();
        bool isNowEmpty = m_HistoryStack.empty();

        m_Mutex.unlock();

        emit canUndoChanged();
        emit undoDescriptionChanged();

        if (isNowEmpty) {
            emit undoStackEmpty();
        }
    } else {
        m_Mutex.unlock();
    }
}
