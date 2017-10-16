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

#include <vector>
#include "../Helpers/localconfig.h"

namespace Helpers {
    class AsyncCoordinator;
}

namespace KeywordsPresets {
    struct PresetData {
        QStringList m_Keywords;
        QString m_Name;
    };

    struct PresetModel;

    class PresetKeywordsModelConfig
    {
    friend class PresetKeywordsModel;

    public:
        PresetKeywordsModelConfig();
        void initializeConfigs();
        void loadFromModel(const std::vector<PresetModel *> &presets);
        void sync();

    private:
        bool processLocalConfig(const QJsonDocument &document);

    public:
        const QVector<PresetData> &getPresetData() const { return m_PresetData; }

#ifdef INTEGRATION_TESTS
    public:
        void initialize(const QVector<PresetData> &presetData) { m_PresetData = presetData; }
#endif

    private:
        void parsePresetArray(const QJsonArray &array);
        void writeToConfig();

    private:
        Helpers::LocalConfig m_Config;
        QString m_LocalConfigPath;
        QVector<PresetData> m_PresetData;
    };
}

#endif // PRESETSKEYWORDSMODELCONFIG_H
