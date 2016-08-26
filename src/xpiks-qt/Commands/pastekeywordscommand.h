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

#ifndef PASTEKEYWORDSCOMMAND_H
#define PASTEKEYWORDSCOMMAND_H

#include <QVector>
#include <vector>
#include <QStringList>
#include "commandbase.h"
#include "../Models/metadataelement.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class PasteKeywordsCommand : public CommandBase
    {
    public:
        PasteKeywordsCommand(std::vector<Models::MetadataElement> &metadataElements,
                             const QStringList &keywords) :
            CommandBase(CommandType::PasteKeywords),
            m_MetadataElements(std::move(metadataElements)),
            m_KeywordsList(keywords)
        {}

        virtual ~PasteKeywordsCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        std::vector<Models::MetadataElement> m_MetadataElements;
        QStringList m_KeywordsList;
    };

    class PasteKeywordsCommandResult : public CommandResult {
    public:
        PasteKeywordsCommandResult(const QVector<int> &indicesToUpdate) :
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
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // PASTEKEYWORDSCOMMAND_H
