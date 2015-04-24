/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include <QStack>
#include "historyitem.h"
#include "../Commands/commandmanager.h"

namespace UndoRedo {
    class UndoRedoManager
    {
    public:
        UndoRedoManager() {}
        ~UndoRedoManager() { qDeleteAll(m_HistoryStack); }

        void setCommandManager(Commands::CommandManager *commandManager) {
            Q_ASSERT(commandManager != NULL); m_CommandManager = commandManager;
        }

        void recordAction(HistoryItem *historyItem);
        bool undoLastAction();
        void discardLastAction();

    private:
        void doUndoAction(HistoryItem *historyItem) const;

    private:
        QStack<HistoryItem*> m_HistoryStack;
        Commands::CommandManager *m_CommandManager;
    };
}

#endif // UNDOREDOMANAGER_H
