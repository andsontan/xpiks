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

#ifndef ADDARTWORKITEM_H
#define ADDARTWORKITEM_H

#include "historyitem.h"

namespace UndoRedo {

   class AddArtworksHistoryItem : public HistoryItem
    {
    public:
        AddArtworksHistoryItem(int firstIndex, int count) :
           HistoryItem(AddedArtworksActionType),
           m_FirstIndex(firstIndex),
           m_Count(count)
        {
            Q_ASSERT(count > 0);
        }

       virtual ~AddArtworksHistoryItem() { }

   public:
        virtual void undo(const Commands::CommandManager *commandManager) const;

   public:
        virtual QString getDescription() const {
            return m_Count > 1 ? QString("%1 items added").arg(m_Count) :
                                 QString("1 item added");
        }

    private:
        int m_FirstIndex;
        int m_Count;
    };
}

#endif // ADDARTWORKITEM_H
