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

#ifndef MODIFYARTWORKHISTORYITEM_H
#define MODIFYARTWORKHISTORYITEM_H

#include <QVector>
#include <QString>
#include "historyitem.h"
#include "artworkmetadatabackup.h"

namespace UndoRedo {
    enum ModificationType {
        PasteModificationType,
        CombinedEditModificationType
    };

    QString getModificationTypeDescription(ModificationType type);

    class ModifyArtworksHistoryItem : public HistoryItem
    {
    public:
        ModifyArtworksHistoryItem(QVector<ArtworkMetadataBackup*> backups,
                                  QVector<int> indices,
                                  ModificationType modificationType) :
            HistoryItem(ModifyArtworksActionType),
            m_ArtworksBackups(backups),
            m_Indices(indices),
            m_ModificationType(modificationType)
        {
            Q_ASSERT(backups.length() == indices.length());
            Q_ASSERT(!backups.empty());
        }

        virtual ~ModifyArtworksHistoryItem() { qDeleteAll(m_ArtworksBackups); }

    public:
         virtual void undo(const Commands::ICommandManager *commandManagerInterface) const;

    public:
         virtual QString getDescription() const {
             int count = m_ArtworksBackups.count();
             QString typeStr = getModificationTypeDescription(m_ModificationType);
             return count > 1 ? QString("(%1)  %2 items modified").arg(typeStr).arg(count) :
                                  QString("(%1)  1 item modified").arg(typeStr);
         }


    private:
        QVector<ArtworkMetadataBackup*> m_ArtworksBackups;
        QVector<int> m_Indices;
        ModificationType m_ModificationType;
    };
}

#endif // MODIFYARTWORKHISTORYITEM_H
