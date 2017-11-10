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
#include "presetgroupsmodel.h"

#define DEFAULT_GROUP_ID -1

namespace Helpers {
    class AsyncCoordinator;
}

namespace KeywordsPresets {
    struct PresetData {
        QStringList m_Keywords;
        QString m_Name;
        int m_GroupID;
    };

    struct PresetGroupData {
        QString m_Name;
        int m_ID;
    };

    struct PresetModel;

    class PresetKeywordsModelConfig
    {
    friend class PresetKeywordsModel;

    public:
        PresetKeywordsModelConfig();
        void initializeConfigs();
        void loadFromModel(const std::vector<PresetModel *> &presets, const std::vector<GroupModel> &presetGroups);
        void sync();

    private:
        void backupXpiks14xPresets(const QString &filepath);
        bool processLocalConfig(const QJsonDocument &document);

    public:
        const std::vector<PresetData> &getPresetData() const { return m_PresetData; }
        const std::vector<PresetGroupData> &getPresetGroupsData() const { return m_PresetGroupsData; }

#ifdef INTEGRATION_TESTS
    public:
        void initialize(const std::vector<PresetData> &presetData) { m_PresetData = presetData; }
#endif

    private:
        void parsePresetArray(const QJsonArray &array);
        void parsePresetGroups(const QJsonArray &array);
        void writeToConfig();

    private:
        Helpers::LocalConfig m_Config;
        QString m_LocalConfigPath;
        std::vector<PresetData> m_PresetData;
        std::vector<PresetGroupData> m_PresetGroupsData;
    };
}

#endif // PRESETSKEYWORDSMODELCONFIG_H
