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

#include "sandboxeddependencies.h"
#include "../Models/uimanager.h"
#include "uiprovider.h"

namespace Plugins {
    UIProviderSafe::UIProviderSafe(int pluginID, UIProvider *realUIProvider):
        m_PluginID(pluginID),
        m_RealUIProvider(realUIProvider)
    {
        Q_ASSERT(realUIProvider != nullptr);
    }

    void UIProviderSafe::openWindow(const QUrl &rcPath, const QHash<QString, QObject *> &contextModels) const {
        m_RealUIProvider->openWindow(rcPath, contextModels);
    }

    int UIProviderSafe::addTab(const QString &tabIconUrl, const QString &tabComponentUrl) const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);
        int result = uiManager->addPluginTab(m_PluginID, tabIconUrl, tabComponentUrl);
        uiManager->updateTabs();
        return result;
    }

    bool UIProviderSafe::removeTab(int tabID) const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);
        bool result = uiManager->removePluginTab(m_PluginID, tabID);
        if (result) {
            uiManager->updateTabs();
        }

        return result;
    }

    std::shared_ptr<QuickBuffer::ICurrentEditable> UIProviderSafe::getCurrentEditable() const {
        auto *uiManager = m_RealUIProvider->getUIManager();
        Q_ASSERT(uiManager != nullptr);

        return uiManager->getCurrentEditable();
    }
}
