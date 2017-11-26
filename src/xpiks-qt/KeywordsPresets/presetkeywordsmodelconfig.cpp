/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "presetkeywordsmodelconfig.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "presetkeywordsmodel.h"
#include "../Common/version.h"

namespace KeywordsPresets {
#define OVERWRITE_KEY QLatin1String("overwrite")
#define PRESETKEYS_KEY QLatin1String("presetkeywords")
#define PRESETGROUPS_KEY QLatin1String("presetgroups")

#define PRESET_NAME_KEY QLatin1String("name")
#define PRESET_KEYWORDS_KEY QLatin1String("keywords")
#define PRESET_GROUP_ID_KEY QLatin1String("group")

#define GROUP_NAME_KEY QLatin1String("name")
#define GROUP_ID_KEY QLatin1String("id")

#ifdef QT_DEBUG
    #ifdef INTEGRATION_TESTS
        #define LOCAL_PRESETKEYWORDS_LIST_FILE "tests_keywords_presets.json"
    #else
        #define LOCAL_PRESETKEYWORDS_LIST_FILE "debug_keywords_presets.json"
    #endif
#else
#define LOCAL_PRESETKEYWORDS_LIST_FILE "keywords_presets.json"
#endif

#define OVERWRITE_PRESETS_CONFIG false

    bool tryParsePresetKeywords(const QJsonValue &presetKeywordsValue, QStringList &keywordsList) {
        bool parsed = false;

        if (presetKeywordsValue.isArray()) {
            QJsonArray jsonArray = presetKeywordsValue.toArray();
            const int size = jsonArray.size();
            keywordsList.reserve(size);

            for (auto item: jsonArray) {
                if (!item.isString()) {
                    LOG_WARNING << "value is not string";
                    continue;
                }

                keywordsList.append(item.toString());
            }

            parsed = !keywordsList.empty();
        }

        return parsed;
    }

    PresetKeywordsModelConfig::PresetKeywordsModelConfig()
    { }

    void PresetKeywordsModelConfig::initializeConfigs() {
        LOG_DEBUG << "#";

        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;
        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_PRESETKEYWORDS_LIST_FILE);
        } else {
            localConfigPath = LOCAL_PRESETKEYWORDS_LIST_FILE;
        }

        if (XPIKS_MAJOR_VERSION_CHECK(1, 5) ||
                XPIKS_MAJOR_VERSION_CHECK(1, 4)) {
            backupXpiks14xPresets(localConfigPath);
        } else {
            // remove backup for 1.6
            Q_ASSERT(false);
        }

        m_Config.initConfig(localConfigPath);
        processLocalConfig(m_Config.getConfig());
    }

    void PresetKeywordsModelConfig::loadFromModel(const std::vector<PresetModel *> &presets,
                                                  const std::vector<GroupModel> &presetGroups) {
        const size_t presetsSize = presets.size();
        LOG_INTEGR_TESTS_OR_DEBUG << presets.size() << "presets;" << presetGroups.size() << "groups";

        m_PresetData.clear();
        m_PresetData.resize(presetsSize);

        for (size_t i = 0; i < presetsSize; i++) {
            auto *item = presets[i];
            auto &name = item->m_PresetName;
            auto &keywordsModel = item->m_KeywordsModel;
            int groupId = item->m_GroupID;

            auto &presetItem = m_PresetData.at(i);

            presetItem.m_Keywords = keywordsModel.getKeywords();
            presetItem.m_Name = name;
            presetItem.m_GroupID = groupId;
        }

        const size_t groupsSize = presetGroups.size();

        m_PresetGroupsData.clear();
        m_PresetGroupsData.resize(groupsSize);

        for (size_t i = 0; i < groupsSize; i++) {
            auto &group = m_PresetGroupsData.at(i);
            group.m_Name = presetGroups[i].m_Name;
            group.m_ID = presetGroups[i].m_GroupID;
        }
    }

    void PresetKeywordsModelConfig::sync() {
        writeToConfig();
    }

    void PresetKeywordsModelConfig::backupXpiks14xPresets(const QString &filepath) {
        LOG_DEBUG << filepath;
        if (!QFileInfo(filepath).exists()) {
            LOG_WARNING << filepath << "does not exist";
            return;
        }

        QString backupPath = filepath + ".14x.backup";

        if (QFileInfo(backupPath).exists()) {
            LOG_DEBUG << "Presets backup already exists";
            return;
        }

        if (QFile::copy(filepath, backupPath)) {
            LOG_DEBUG << "Presets backed up to" << backupPath;
        } else {
            LOG_WARNING << "Failed to backup presets to" << backupPath;
        }
    }

    bool PresetKeywordsModelConfig::processLocalConfig(const QJsonDocument &document) {
        LOG_INTEGR_TESTS_OR_DEBUG << document;
        bool anyError = false;

        do {
            if (!document.isObject()) {
                LOG_WARNING << "Json document is not an object";
                anyError = true;
                break;
            }

            QJsonObject rootObject = document.object();
            if (!rootObject.contains(PRESETKEYS_KEY)) {
                LOG_WARNING << "There's no presetKeys key in json";
                anyError = true;
                break;
            }

            QJsonValue presetArrayValue = rootObject.value(PRESETKEYS_KEY);
            if (!presetArrayValue.isArray()) {
                LOG_WARNING << "presetArray array object is not an array";
                anyError = true;
                break;
            }

            QJsonArray presetArray = presetArrayValue.toArray();
            parsePresetArray(presetArray);

            QJsonValue presetGroupsValue = rootObject.value(PRESETGROUPS_KEY);
            if (presetGroupsValue.isArray()) {
                QJsonArray groupsArray = presetGroupsValue.toArray();
                parsePresetGroups(groupsArray);
            }
        } while (false);

        return anyError;
    }

    void PresetKeywordsModelConfig::parsePresetArray(const QJsonArray &array) {
        const int size = array.size();

        for (int i = 0; i < size; ++i) {
            QJsonValue item = array.at(i);

            if (!item.isObject()) { continue; }

            QJsonObject presetKeywordsItem = item.toObject();

            // legacy format
            if (presetKeywordsItem.size() == 1) {
                QString presetName = presetKeywordsItem.keys()[0];
                QJsonValue presetKeywordsValue = presetKeywordsItem.value(presetName);
                QStringList keywords;

                if (tryParsePresetKeywords(presetKeywordsValue, keywords)) {
                    m_PresetData.push_back({keywords, presetName, DEFAULT_GROUP_ID});
                }
            } else {
                QString presetName;
                QJsonValue presetNameValue = presetKeywordsItem.value(PRESET_NAME_KEY);
                if (presetNameValue.isString()) {
                    presetName = presetNameValue.toString().trimmed();
                } else {
                    continue;
                }

                if (presetName.isEmpty()) { continue; }

                QJsonValue keywordsValue = presetKeywordsItem.value(PRESET_KEYWORDS_KEY);
                QStringList keywords;
                if (!tryParsePresetKeywords(keywordsValue, keywords)) { continue; }

                QJsonValue groupIdValue = presetKeywordsItem.value(PRESET_GROUP_ID_KEY);
                int groupID = DEFAULT_GROUP_ID;
                if (groupIdValue.isDouble()) {
                    groupID = groupIdValue.toInt(DEFAULT_GROUP_ID);
                }

                m_PresetData.push_back({keywords, presetName, groupID});
            }
        }
    }

    void PresetKeywordsModelConfig::parsePresetGroups(const QJsonArray &array) {
        const int size = array.size();

        for (int i = 0; i < size; ++i) {
            QJsonValue item = array.at(i);

            if (!item.isObject()) { continue; }

            QJsonObject groupItem = item.toObject();

            QJsonValue groupNameValue = groupItem.value(GROUP_NAME_KEY);
            if (!groupNameValue.isString()) { continue; }
            QString groupName = groupNameValue.toString().trimmed();
            if (groupName.isEmpty()) { continue; }

            QJsonValue groupIdValue = groupItem.value(GROUP_ID_KEY);
            if (!groupIdValue.isDouble()) { continue; }
            int groupId = groupIdValue.toInt(DEFAULT_GROUP_ID);

            m_PresetGroupsData.push_back({groupName, groupId});
        }
    }

    void PresetKeywordsModelConfig::writeToConfig() {
        LOG_DEBUG << "#";
        QJsonArray presetsArray;

        for (auto &item: m_PresetData) {
            QJsonObject object;
            QJsonArray keywords;
            keywords = QJsonArray::fromStringList(item.m_Keywords);

            object.insert(PRESET_NAME_KEY, item.m_Name);
            object.insert(PRESET_KEYWORDS_KEY, keywords);
            object.insert(PRESET_GROUP_ID_KEY, item.m_GroupID);

            presetsArray.append(object);
        }

        QJsonArray groupsArray;
        for (auto &item: m_PresetGroupsData) {
            QJsonObject object;

            object.insert(GROUP_NAME_KEY, item.m_Name);
            object.insert(GROUP_ID_KEY, item.m_ID);

            groupsArray.append(object);
        }

        QJsonObject rootObject;
        rootObject.insert(OVERWRITE_KEY, OVERWRITE_PRESETS_CONFIG);
        rootObject.insert(PRESETKEYS_KEY, presetsArray);
        rootObject.insert(PRESETGROUPS_KEY, groupsArray);

        QJsonDocument doc;
        doc.setObject(rootObject);

        Helpers::LocalConfigDropper dropper(&m_Config);
        Q_UNUSED(dropper);

        m_Config.setConfig(doc);
        m_Config.saveToFile();
    }
}
