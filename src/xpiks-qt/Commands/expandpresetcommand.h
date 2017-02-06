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

#ifndef EXPANDPRESETCOMMAND_H
#define EXPANDPRESETCOMMAND_H

#include <QVector>
#include <vector>
#include <QStringList>
#include "commandbase.h"
#include "../Models/metadataelement.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class ExpandPresetCommand : public CommandBase
    {
    public:
        ExpandPresetCommand(Models::MetadataElement &&metadataElement,
                            int presetIndex, int keywordIndex = -1):
            CommandBase(CommandType::ExpandPreset),
            m_MetadataElement(std::move(metadataElement)),
            m_PresetIndex(presetIndex),
            m_KeywordIndex(keywordIndex)
        { }

        virtual ~ExpandPresetCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        Models::MetadataElement m_MetadataElement;
        int m_PresetIndex;
        int m_KeywordIndex;
    };

    class ExpandPresetCommandResult : public CommandResult {
    public:
        ExpandPresetCommandResult(int indexToUpdate) :
            m_IndexToUpdate(indexToUpdate)
        {
        }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const override;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        int m_IndexToUpdate;
    };
}

#endif // EXPANDPRESETCOMMAND_H
