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

#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include "ihistoryitem.h"

class QString;

namespace Commands {
    class CommandManager;
}

namespace UndoRedo {

    enum struct HistoryActionType {
        AddArtworks,
        RemovedArtworks,
        ModifyArtworks
    };

    class HistoryItem : public IHistoryItem
    {
    public:
        HistoryItem(HistoryActionType actionType, int commandID) :
            m_ActionType(actionType),
            m_CommandID(commandID)
        {}
        virtual ~HistoryItem() {}

    public:
        virtual int getActionType() const override { return (int)m_ActionType; }
        virtual int getCommandID() const override { return m_CommandID; }

    private:
        HistoryActionType m_ActionType;
        int m_CommandID;
    };
}

#endif // HISTORYITEM_H
