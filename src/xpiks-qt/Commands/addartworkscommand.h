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

#ifndef ADDARTWORKSCOMMAND_H
#define ADDARTWORKSCOMMAND_H

#include <QStringList>
#include <QHash>
#include "commandbase.h"

namespace Commands {
    class AddArtworksCommand : public CommandBase
    {
    public:
        AddArtworksCommand(const QStringList &pathes, const QStringList &vectorPathes, bool autoDetectVectors) :
            CommandBase(CommandType::AddArtworks),
            m_FilePathes(pathes),
            m_VectorsPathes(vectorPathes),
            m_AutoDetectVectors(autoDetectVectors)
        {}

        virtual ~AddArtworksCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        void decomposeVectors(QHash<QString, QHash<QString, QString> > &vectors) const;

    public:
        QStringList m_FilePathes;
        QStringList m_VectorsPathes;
        bool m_AutoDetectVectors;
    };

    class AddArtworksCommandResult : public CommandResult {
    public:
        AddArtworksCommandResult(int count):
        m_NewFilesAdded(count)
        {}
    public:
        int m_NewFilesAdded;
    };
}

#endif // ADDARTWORKSCOMMAND_H
