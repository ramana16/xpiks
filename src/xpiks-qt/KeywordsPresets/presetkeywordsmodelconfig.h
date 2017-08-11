/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PRESETSKEYWORDSMODELCONFIG_H
#define PRESETSKEYWORDSMODELCONFIG_H

#include "../Models/abstractconfigupdatermodel.h"
#include "presetkeywordsmodel.h"

namespace Helpers {
    class AsyncCoordinator;
}

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
        void initializeConfigs(Helpers::AsyncCoordinator *initCoordinator);
        void loadFromModel(const std::vector<PresetModel *> &presets);
        void sync();

        // AbstractConfigUpdaterModel interface
    protected:
        virtual bool processLocalConfig(const QJsonDocument &document) override;
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
