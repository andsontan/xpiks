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

#ifndef ADDARTWORKITEM_H
#define ADDARTWORKITEM_H

#include <QObject>
#include <QVector>
#include <QPair>
#include "historyitem.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/defines.h"

namespace UndoRedo {
   class AddArtworksHistoryItem : public HistoryItem
    {
    public:
        AddArtworksHistoryItem(int firstIndex, int count) :
           HistoryItem(HistoryActionType::AddArtworks)
        {
            Q_ASSERT(count > 0);
            m_AddedRanges.append(qMakePair(firstIndex, firstIndex + count - 1));
        }

        AddArtworksHistoryItem(const QVector<QPair<int, int> > &rangesAdded) :
           HistoryItem(HistoryActionType::AddArtworks),
           m_AddedRanges(rangesAdded)
        {
            Q_ASSERT(!rangesAdded.empty());
        }

       virtual ~AddArtworksHistoryItem() { LOG_DEBUG << "#"; }

   public:
        virtual void undo(const Commands::ICommandManager *commandManagerInterface) const;

   public:
        virtual QString getDescription() const {
            int count = Helpers::getRangesLength(m_AddedRanges);
            return count > 1 ? QObject::tr("%1 items added").arg(count) :
                                 QObject::tr("1 item added");
        }

    private:
        QVector<QPair<int, int> > m_AddedRanges;
    };
}

#endif // ADDARTWORKITEM_H
