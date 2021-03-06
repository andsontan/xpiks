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

#ifndef ABSTRACTUPDATERCONFIGMODEL_H
#define ABSTRACTUPDATERCONFIGMODEL_H

#include <QSet>
#include <QJsonDocument>
#include "../Helpers/comparevaluesjson.h"
#include "../Helpers/remoteconfig.h"
#include "../Helpers/localconfig.h"
#include "../Helpers/jsonhelper.h"
#include "../Common/baseentity.h"

namespace Models {
    class AbstractConfigUpdaterModel: public QObject, public Helpers::CompareValuesJson, public Common::BaseEntity
    {
        Q_OBJECT
    public:
        AbstractConfigUpdaterModel(bool forceOverwrite, QObject *parent=nullptr);
        virtual ~AbstractConfigUpdaterModel() {}

    public:
        void initializeConfigs(const QString &configUrl, const QString &filePath);
        const Helpers::LocalConfig &getLocalConfig() const { return m_LocalConfig; }
        Helpers::LocalConfig &getLocalConfig() { return m_LocalConfig; }

    private slots:
        void remoteConfigArrived();

    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal);
        virtual bool parseConfig(const QJsonDocument &document) = 0;
    private:
        virtual void initRemoteConfig(const QString &configUrl);
        virtual void initLocalConfig(const QString &filePath);

    private:
        Helpers::RemoteConfig m_RemoteConfig;
        Helpers::LocalConfig m_LocalConfig;
        bool m_ForceOverwrite;
    };
}
#endif // QABSTRACTUPDATERCONFIGMODEL_H
