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

#ifndef PLUGINWRAPPER_H
#define PLUGINWRAPPER_H

#include <QObject>
#include <QString>
#include "pluginactionsmodel.h"
#include "sandboxeddependencies.h"
#include "iuiprovider.h"

namespace Plugins {
    class XpiksPluginInterface;
    class UIProvider;

    class PluginWrapper
    {
    public:
        PluginWrapper(XpiksPluginInterface *pluginInterface, int pluginID, UIProvider *realUIProvider);

    public:
        int getPluginID() const { return m_PluginID; }
        bool getIsEnabled() const { return m_IsEnabled; }
        const QString &getPrettyName() const { return m_PrettyName; }
        const QString &getVersionString() const { return m_VersionString; }
        const QString &getAuthor() const { return m_Author; }

        bool anyActionsProvided() const { return m_ActionsModel.size() > 0; }
        PluginActionsModel *getActionsModel() { return &m_ActionsModel; }
        IUIProvider *getUIProvider() { return &m_UIProviderSafe; }

        void enablePlugin();
        void disablePlugin();

        void triggerActionSafe(int actionID) const;
        void finalizePlugin();

    private:
        XpiksPluginInterface *m_PluginInterface;
        PluginActionsModel m_ActionsModel;
        UIProviderSafe m_UIProviderSafe;
        int m_PluginID;
        bool m_IsEnabled;
        const QString &m_PrettyName;
        const QString &m_VersionString;
        const QString &m_Author;
    };
}

#endif // PLUGINWRAPPER_H
