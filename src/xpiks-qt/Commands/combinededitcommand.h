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

#ifndef COMBINEDEDITCOMMAND_H
#define COMBINEDEDITCOMMAND_H

#include <QStringList>
#include <QString>
#include <vector>
#include <QVector>
#include "commandbase.h"
#include "../Models/metadataelement.h"
#include "../Common/flags.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {

    class CombinedEditCommand: public CommandBase
    {
    public:
        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            std::vector<Models::MetadataElement> &infos,
                            const QString &description, const QString &title,
                            const QStringList &keywords) :
            CommandBase(CommandType::CombinedEdit),
            m_MetadataElements(std::move(infos)),
            m_ArtworkDescription(description),
            m_ArtworkTitle(title),
            m_Keywords(keywords),
            m_EditFlags(editFlags)
        { }

        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            std::vector<Models::MetadataElement> &infos) :
            CommandBase(CommandType::CombinedEdit),
            m_MetadataElements(std::move(infos)),
            m_EditFlags(editFlags)
        { }

        virtual ~CombinedEditCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        void setKeywords(Models::ArtworkMetadata *metadata) const;
        void setDescription(Models::ArtworkMetadata *metadata) const;
        void setTitle(Models::ArtworkMetadata *metadata) const;

    private:
        std::vector<Models::MetadataElement> m_MetadataElements;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QStringList m_Keywords;
        Common::CombinedEditFlags m_EditFlags;
    };

    class CombinedEditCommandResult : public CommandResult {
    public:
        CombinedEditCommandResult(const QVector<Models::ArtworkMetadata *> &affectedItems,
                                  const QVector<Models::ArtworkMetadata *> &itemsToSave,
                                  const QVector<int> &indicesToUpdate) :
            m_AffectedItems(affectedItems),
            m_ItemsToSave(itemsToSave),
            m_IndicesToUpdate(indicesToUpdate)
        {
        }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        QVector<Models::ArtworkMetadata *> m_AffectedItems;
        QVector<Models::ArtworkMetadata *> m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // COMBINEDEDITCOMMAND_H
