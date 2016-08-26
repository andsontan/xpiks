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

#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include "icommandbase.h"

namespace Commands {

    enum struct CommandType {
        AddArtworks = 0,
        RemoveArtworks = 1,
        CombinedEdit = 2,
        PasteKeywords = 3,
        FindAndReplace = 4,
        DeleteKeywords = 5
    };

    class CommandResult;
    class ICommandManager;

    class CommandBase : public ICommandBase
    {
    public:
        CommandBase(CommandType commandType):
            m_CommandType(commandType)
        {}
        virtual ~CommandBase() {}

    public:
        virtual int getCommandType() const { return (int)m_CommandType; }

    private:
        CommandType m_CommandType;
    };

    class CommandResult : public ICommandResult {
    public:
        CommandResult() {}
        virtual ~CommandResult() {}

    public:
        virtual int getStatus() const { return 0; }
    };
}

#endif // COMMANDBASE_H
