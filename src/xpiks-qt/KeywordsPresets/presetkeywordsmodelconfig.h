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

#ifndef PRESETSKEYWORDSMODELCONFIG_H
#define PRESETSKEYWORDSMODELCONFIG_H

#include "../Models/abstractconfigupdatermodel.h"
#include "presetkeywordsmodel.h"

namespace KeywordsPresets {
    struct PresetData {
        QStringList m_Keywords;
        QString m_Name;
    };

    class PresetKeywordsModelConfig:
        public Models::AbstractConfigUpdaterModel
    {
        Q_OBJECT
    friend class PresetKeywordsModel;

    public:
        PresetKeywordsModelConfig(QObject *parent=0);
        void initializeConfigs();
        void saveFromModel(const std::vector<PresetModel *> &presets);

    public:
        // AbstractConfigUpdaterModel interface

    protected:
        virtual bool parseConfig(const QJsonDocument &document) override;

        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) override;
        // CompareValuesJson interface

    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2) override;

    public:
        void initialize(const QVector<PresetData> &presetData);
        const QVector<PresetData> &getPresetData() const { return m_PresetData; }

    signals:
        void presetsUpdated();

    private:
        void parsePresetArray(const QJsonArray &array);
        void writeToConfig();

    private:
        QString m_LocalConfigPath;
        QVector<PresetData> m_PresetData;
    };
}

#endif // PRESETSKEYWORDSMODELCONFIG_H
