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

#ifndef IHISTORYITEM_H
#define IHISTORYITEM_H

#include <QtPlugin>
#include "../Commands/icommandmanager.h"

namespace UndoRedo {
    class IHistoryItem {
    public:
        virtual void undo(const Commands::ICommandManager *commandManager) const = 0;
        virtual QString getDescription() const = 0;
        virtual int getActionType() const = 0;
        virtual ~IHistoryItem() {}
    };
}

#define IHistoryItem_iid "Xpiks.UndoRedo.IHistoryItem.v0.1"
Q_DECLARE_INTERFACE(UndoRedo::IHistoryItem, IHistoryItem_iid)

#endif // IHISTORYITEM_H
