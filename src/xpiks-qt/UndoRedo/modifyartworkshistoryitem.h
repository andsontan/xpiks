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

#ifndef MODIFYARTWORKHISTORYITEM_H
#define MODIFYARTWORKHISTORYITEM_H

#include <QList>
#include "historyitem.h"
#include "artworkmetadatabackup.h"

namespace UndoRedo {
    class ModifyArtworksHistoryItem : public HistoryItem
    {
    public:
        ModifyArtworksHistoryItem(QList<ArtworkMetadataBackup*> backups,
                                  QList<int> indices) :
            HistoryItem(ModifyArtworksActionType),
            m_ArtworksBackups(backups),
            m_Indices(indices)
        {
            Q_ASSERT(backups.length() == indices.length());
            Q_ASSERT(!backups.empty());
        }

        virtual ~ModifyArtworksHistoryItem() { qDeleteAll(m_ArtworksBackups); }

    public:
         virtual void undo(const Commands::CommandManager *commandManager) const;

    public:
         virtual QString getDescription() const {
             int count = m_ArtworksBackups.count();
             return count > 1 ? QString("%1 items modified").arg(count) :
                                  QString("1 item modified");
         }


    private:
        QList<ArtworkMetadataBackup*> m_ArtworksBackups;
        QList<int> m_Indices;
    };
}

#endif // MODIFYARTWORKHISTORYITEM_H
