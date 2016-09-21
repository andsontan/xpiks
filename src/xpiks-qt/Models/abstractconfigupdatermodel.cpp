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

#include "abstractconfigupdatermodel.h"

namespace Models {
    AbstractConfigUpdaterModel::AbstractConfigUpdaterModel(bool forceOverwrite, QObject *parent):
        QObject(parent),
        m_RemoteConfig(this),
        m_ForceOverwrite(forceOverwrite)
    {
        QObject::connect(&m_RemoteConfig, SIGNAL(configArrived()), this, SLOT(remoteConfigArrived()));
    }

    void AbstractConfigUpdaterModel::initializeConfigs(const QString &configUrl, const QString &filePath) {
        LOG_DEBUG << "#";
        m_LocalConfig.initConfig(filePath);
        m_RemoteConfig.requestInitConfig(configUrl);
    }

    void AbstractConfigUpdaterModel::remoteConfigArrived() {
        LOG_DEBUG << "#";
        const QJsonDocument &remoteDocument = m_RemoteConfig.getConfig();
        processRemoteConfig(remoteDocument, m_ForceOverwrite);

        const QJsonDocument &localDocument = m_LocalConfig.getConfig();
        parseConfig(localDocument);
    }

    void AbstractConfigUpdaterModel::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        LOG_DEBUG << "#";
        QJsonDocument &localDocument = m_LocalConfig.getConfig();
        Helpers::mergeJson(remoteDocument, localDocument, overwriteLocal, *this);
        m_LocalConfig.saveToFile();
    }
}
