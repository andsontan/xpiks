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

#ifndef REMOVEARTWORKSCOMMAND_H
#define REMOVEARTWORKSCOMMAND_H

#include <QVector>
#include <QPair>
#include "commandbase.h"
#include "../Helpers/indiceshelper.h"

namespace Commands {
    class RemoveArtworksCommand : public CommandBase
    {
    public:
        RemoveArtworksCommand(const QVector<QPair<int, int> > &rangesToRemove):
            CommandBase(CommandType::RemoveArtworks),
            m_RangesToRemove(rangesToRemove)
        {
        }

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        QVector<QPair<int, int> > m_RangesToRemove;
    };

    class RemoveArtworksCommandResult : public CommandResult {
    public:
        RemoveArtworksCommandResult(int removedCount):
            m_RemovedArtworksCount(removedCount)
        {}
    public:
        int m_RemovedArtworksCount;
    };
}

#endif // REMOVEARTWORKSCOMMAND_H
