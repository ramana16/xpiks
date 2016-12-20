#ifndef PRESETSKEYWORDSMODELCONFIG_H
#define PRESETSKEYWORDSMODELCONFIG_H

#include "../Models/abstractconfigupdatermodel.h"
#include "presetkeywordsmodel.h"

namespace  Presets {
    struct PresetData {
        QStringList keys;
        QString name;
    };

    class PresetKeywordsModelConfig:
        public Models::AbstractConfigUpdaterModel
    {
    Q_OBJECT
    friend class PresetKeywordsModel;

    public:
        PresetKeywordsModelConfig(QObject *parent=0);
        void initializeConfigs();
        void saveFromModel(const QVector<Preset> &presets);

    public:
        // AbstractConfigUpdaterModel interface

    protected:
        virtual bool parseConfig(const QJsonDocument &document);

        virtual void processRemoteConfig(const QJsonDocument &, bool) {}
        // CompareValuesJson interface

    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2);

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
