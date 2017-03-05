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
        DeleteKeywords = 5,
        ExpandPreset = 6
    };

    class CommandResult;
    class ICommandManager;

    class CommandBase : public ICommandBase
    {
    public:
        CommandBase(CommandType commandType):
            m_CommandType(commandType),
            m_CommandID(-1)
        {}
        virtual ~CommandBase() {}

    public:
        virtual int getCommandType() const override { return (int)m_CommandType; }
        virtual int getCommandID() const override { return m_CommandID; }
        virtual void assignCommandID(int commandID) override { Q_ASSERT(m_CommandID == -1); m_CommandID = commandID; }

    private:
        CommandType m_CommandType;
        int m_CommandID;
    };

    class CommandResult : public ICommandResult {
    public:
        CommandResult() {}
        virtual ~CommandResult() {}

    public:
        virtual int getStatus() const override { return 0; }
    };
}

#endif // COMMANDBASE_H
