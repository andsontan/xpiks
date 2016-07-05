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

#ifndef UNDOREDOMANAGER_H
#define UNDOREDOMANAGER_H

#include <QObject>
#include <stack>
#include <memory>
#include <QMutex>
#include "../Commands/commandmanager.h"
#include "../Common/baseentity.h"
#include "iundoredomanager.h"

namespace UndoRedo {
    class HistoryItem;

    class UndoRedoManager:
            public QObject,
            public Common::BaseEntity,
            public IUndoRedoManager
    {
        Q_OBJECT
        Q_PROPERTY(bool canUndo READ getCanUndo NOTIFY canUndoChanged)
        Q_PROPERTY(QString undoDescription READ getUndoDescription NOTIFY undoDescriptionChanged)
    public:
        UndoRedoManager(QObject *parent=0):
            QObject(parent),
            Common::BaseEntity()
        {}

        virtual ~UndoRedoManager();

    public:
        bool getCanUndo() const { return !m_HistoryStack.empty(); }

    signals:
        void canUndoChanged();
        void undoDescriptionChanged();
        void itemRecorded();
        void undoStackEmpty();

    private:
        QString getUndoDescription() const { return m_HistoryStack.empty() ? "" : m_HistoryStack.top()->getDescription(); }

    public:
        virtual void recordHistoryItem(std::unique_ptr<UndoRedo::IHistoryItem> &historyItem);
        Q_INVOKABLE bool undoLastAction();
        Q_INVOKABLE void discardLastAction();

    private:
        // stack for future todos
        std::stack<std::unique_ptr<IHistoryItem> > m_HistoryStack;
        QMutex m_Mutex;
    };
}

#endif // UNDOREDOMANAGER_H
