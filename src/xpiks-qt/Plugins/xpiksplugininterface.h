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

#ifndef XPIKSPLUGININTERFACE_H
#define XPIKSPLUGININTERFACE_H

#include <QString>
#include <QtPlugin>
#include <QVector>
#include "../Commands/icommandmanager.h"
#include "../UndoRedo/iundoredomanager.h"
#include "ipluginaction.h"
#include "iuiprovider.h"
#include "../Common/iartworkssource.h"

namespace Plugins {
    class XpiksPluginInterface {
    public:
        virtual ~XpiksPluginInterface() {}

    public:
        virtual const QString &getPrettyName() const = 0;
        virtual const QString &getVersionString() const = 0;
        virtual const QString &getAuthor() const = 0;

    public:
        virtual const QVector<IPluginAction*> &getExportedActions() const = 0;
        virtual bool executeAction(int actionID) = 0;

    public:
        virtual void initializePlugin() = 0;
        virtual void finalizePlugin() = 0;

    public:
        virtual void injectCommandManager(Commands::ICommandManager *commandManager) = 0;
        virtual void injectUndoRedoManager(UndoRedo::IUndoRedoManager *undoRedoManager) = 0;
        virtual void injectArtworksSource(Common::IArtworksSource *artworksSource) = 0;
        virtual void injectUIProvider(IUIProvider *uiProvider) = 0;
    };
}

#define XpiksPluginInterface_iid "Xpiks.Plugins.XpiksPluginInterface.v0.1"
Q_DECLARE_INTERFACE(Plugins::XpiksPluginInterface, XpiksPluginInterface_iid)

#endif // XPIKSPLUGININTERFACE_H
