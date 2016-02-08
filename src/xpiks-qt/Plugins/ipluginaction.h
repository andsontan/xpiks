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

#ifndef IPLUGINACTION
#define IPLUGINACTION

#include <QString>
#include <QtPlugin>

namespace Plugins {
    class IPluginAction {
    public:
        virtual ~IPluginAction() {}
        virtual const QString &getActionName() const = 0;
        virtual int getActionID() const = 0;
    };
}

#define IPluginAction_iid "Xpiks.Plugins.IPluginAction.v0.1"
Q_DECLARE_INTERFACE(Plugins::IPluginAction, IPluginAction_iid)

#endif // IPLUGINACTION

