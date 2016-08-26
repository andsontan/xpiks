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

#ifndef FINDANDREPLACECOMMAND_H
#define FINDANDREPLACECOMMAND_H

#include "commandbase.h"
#include <QString>
#include <QVector>
#include "../Common/flags.h"

namespace Models {
    class PreviewMetadataElement;
    class ArtworkMetadata;
}

namespace Commands {
    class FindAndReplaceCommand:
        public CommandBase
    {
    public:
        FindAndReplaceCommand(std::vector<Models::PreviewMetadataElement> &metadataElements,
        const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags):
            CommandBase(CommandType::FindAndReplace),
            m_MetadataElements(std::move(metadataElements)),
            m_ReplaceWhat(replaceWhat),
            m_ReplaceTo(replaceTo),
            m_Flags(flags)
        {}

        virtual ~FindAndReplaceCommand();

    public:
        std::shared_ptr<Commands::ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        std::vector<Models::PreviewMetadataElement> m_MetadataElements;
        QString m_ReplaceWhat;
        QString m_ReplaceTo;
        Common::SearchFlags m_Flags;
    };

    class FindAndReplaceCommandResult:
        public CommandResult
    {
    public:
        FindAndReplaceCommandResult(
        const QVector<Models::ArtworkMetadata *> &itemsToSave,
        const QVector<int> &indicesToUpdate):
            m_ItemsToSave(itemsToSave),
            m_IndicesToUpdate(indicesToUpdate)
        {}

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const;

#ifndef CORE_TESTS

    private:
#else

    public:
#endif
        QVector<Models::ArtworkMetadata *> m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // FINDANDREPLACE_H
