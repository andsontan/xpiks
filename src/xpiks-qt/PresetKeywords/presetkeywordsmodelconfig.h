#ifndef PRESETSKEYWORDSMODELCONFIG_H
#define PRESETSKEYWORDSMODELCONFIG_H

#include "../Models/abstractconfigupdatermodel.h"
#include "presetkeywordsmodel.h"

namespace  KeywordsPreset {
    struct PresetData {
        QStringList m_Keys;
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
        void saveFromModel(const std::vector<Preset> &presets);

    public:
        // AbstractConfigUpdaterModel interface

    protected:
        virtual bool parseConfig(const QJsonDocument &document);

        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal);
        // CompareValuesJson interface

    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2);

    public:
        void initialize(const QVector<PresetData> &presetData);
        const QVector<PresetData> & getPresetData();

    signals:
        void presetsUpdated();

    private:
        void parsePresetArray(const QJsonArray &array);
        void writeToConfig();

    private:
        QString localConfigPath;
        QVector<PresetData> m_PresetData;
    };
}

#endif // PRESETSKEYWORDSMODELCONFIG_H
