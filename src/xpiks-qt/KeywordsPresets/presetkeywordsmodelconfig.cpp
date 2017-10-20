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

namespace KeywordsPresets {
#define OVERWRITE_KEY QLatin1String("overwrite")
#define PRESETKEYS_KEY QLatin1String("presetkeywords")

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

    PresetKeywordsModelConfig::PresetKeywordsModelConfig()
    {}

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

        m_Config.initConfig(localConfigPath);
        processLocalConfig(m_Config.getConfig());
    }

    void PresetKeywordsModelConfig::loadFromModel(const std::vector<PresetModel *> &presets) {
        int size = (int)presets.size();
        LOG_INTEGR_TESTS_OR_DEBUG << size;

        m_PresetData.clear();
        m_PresetData.resize(size);

        for (int i = 0; i < size; i++) {
            auto *item = presets[i];
            auto &name = item->m_PresetName;
            auto &keywordsModel = item->m_KeywordsModel;

            m_PresetData[i].m_Keywords = keywordsModel.getKeywords();
            m_PresetData[i].m_Name = name;
        }
    }

    void PresetKeywordsModelConfig::sync() {
        writeToConfig();
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

            QJsonValue presetArrayValue = rootObject[PRESETKEYS_KEY];
            if (!presetArrayValue.isArray()) {
                LOG_WARNING << "presetArray array object is not an array";
                anyError = true;
                break;
            }

            QJsonArray presetArray = presetArrayValue.toArray();
            parsePresetArray(presetArray);
        } while (false);

        return anyError;
    }

    void PresetKeywordsModelConfig::parsePresetArray(const QJsonArray &array) {
        QStringList keys;
        int size = array.size();

        keys.reserve(size);

        for (int i = 0; i < size; ++i) {
            QJsonValue item = array.at(i);

            if (!item.isObject()) {
                continue;
            }

            QJsonObject presetKeywordsItem = item.toObject();
            if (presetKeywordsItem.size() != 1) {
                continue;
            }

            QString presetKey = presetKeywordsItem.keys()[0];
            QJsonValue presetKeywordsValue = presetKeywordsItem.value(presetKey);

            if (!presetKeywordsValue.isArray()) {
                continue;
            }

            QJsonArray jsonArray = presetKeywordsValue.toArray();
            QStringList list;
            int size = jsonArray.size();
            list.reserve(size);

            for (auto item: jsonArray) {
                if (!item.isString()) {
                    LOG_WARNING << "value is not string";
                    continue;
                }

                list.append(item.toString());
            }

            m_PresetData.push_back({list, presetKey});
        }
    }

    void PresetKeywordsModelConfig::writeToConfig() {
        LOG_DEBUG << "#";
        QJsonArray jsonArray;

        for (auto &item: m_PresetData) {
            QJsonObject object;
            QJsonArray keywords;
            keywords = QJsonArray::fromStringList(item.m_Keywords);
            object.insert(item.m_Name, keywords);
            jsonArray.append(object);
        }

        QJsonObject rootObject;
        rootObject.insert(OVERWRITE_KEY, OVERWRITE_PRESETS_CONFIG);
        rootObject.insert(PRESETKEYS_KEY, jsonArray);
        QJsonDocument doc;
        doc.setObject(rootObject);

        Helpers::LocalConfigDropper dropper(&m_Config);
        Q_UNUSED(dropper);

        m_Config.setConfig(doc);
        m_Config.saveToFile();
    }
}
