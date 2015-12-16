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

#include "undoredomanager.h"
#include <QDebug>

UndoRedo::UndoRedoManager::~UndoRedoManager() { qDeleteAll(m_HistoryStack); }

void UndoRedo::UndoRedoManager::recordHistoryItem(UndoRedo::HistoryItem *historyItem) {
    qInfo() << "History item about to be recorded:" << historyItem->getActionType();

    QMutexLocker locker(&m_Mutex);

    if (!m_HistoryStack.empty()) {
        HistoryItem *oldItem = m_HistoryStack.pop();
        delete oldItem;
    }

    m_HistoryStack.append(historyItem);
    emit canUndoChanged();
    emit itemRecorded();
    emit undoDescriptionChanged();
}

bool UndoRedo::UndoRedoManager::undoLastAction()
{
    m_Mutex.lock();

    bool anyItem = false;
    anyItem = !m_HistoryStack.empty();

    if (anyItem) {
        HistoryItem *historyItem = m_HistoryStack.pop();
        m_Mutex.unlock();

        emit canUndoChanged();
        emit undoDescriptionChanged();
        historyItem->undo(m_CommandManager);
        delete historyItem;
    } else {
        m_Mutex.unlock();
    }

    return anyItem;
}

void UndoRedo::UndoRedoManager::discardLastAction()
{
    m_Mutex.lock();

    bool anyItem = false;
    anyItem = !m_HistoryStack.empty();

    if (anyItem) {
        HistoryItem *historyItem = m_HistoryStack.pop();
        m_Mutex.unlock();

        emit canUndoChanged();
        emit undoDescriptionChanged();
        delete historyItem;
    } else {
        m_Mutex.unlock();
    }
}
