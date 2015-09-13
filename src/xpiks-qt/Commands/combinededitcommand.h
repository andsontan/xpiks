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

#ifndef COMBINEDEDITCOMMAND_H
#define COMBINEDEDITCOMMAND_H

#include <QList>
#include <QString>
#include <QStringList>
#include "commandbase.h"

namespace Models { class ArtItemInfo; }
namespace UndoRedo { class ArtworkMetadataBackup; }

namespace Commands {

    enum CombinedEditType { AppendEditType, SetEditType };

    class CombinedEditCommand: public CommandBase
    {
    public:
        CombinedEditCommand(CombinedEditType editType,
                            const QList<Models::ArtItemInfo*> &infos,
                            QString description, QString title,
                            QStringList keywords) :
            CommandBase(CombinedEditCommandType),
            m_ArtItemInfos(infos),
            m_EditType(editType),
            m_ArtworkDescription(description),
            m_ArtworkTitle(title),
            m_Keywords(keywords)
        { }

        virtual ~CombinedEditCommand() {}

    public:
        virtual CommandResult *execute(const CommandManager *commandManager) const;

    private:
        void saveSetKeywords(QList<UndoRedo::ArtworkMetadataBackup*> &backups,
                             QList<int> &indicesToUpdate) const;
        void saveAppendKeywords(QList<UndoRedo::ArtworkMetadataBackup*> &backups,
                                QList<int> &indicesToUpdate) const;

    private:
        QList<Models::ArtItemInfo*> m_ArtItemInfos;
        CombinedEditType m_EditType;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QStringList m_Keywords;
    };

    class CombinedEditCommandResult : public CommandResult {
    public:
        CombinedEditCommandResult(const QList<int> &indicesToUpdate) :
            m_IndicesToUpdate(indicesToUpdate)
        {
        }

    public:
        QList<int> m_IndicesToUpdate;
        // list of Ids to update!
    };
}

#endif // COMBINEDEDITCOMMAND_H
