#include "presetkeywordsmodelconfig.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace  Preset {
#define PRESETKEYS_KEY QLatin1String("presetkeys")
#define LOCAL_PRESETKEYWORDS_LIST_FILE "preset_keywords.json"

    PresetKeywordsModelConfig::PresetKeywordsModelConfig(QObject *parent):
        Models::AbstractConfigUpdaterModel(false, parent)
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

        LOG_DEBUG << "tasha path " << localConfigPath;

        AbstractConfigUpdaterModel::initializeConfigs(localConfigPath);
        const Helpers::LocalConfig &localConfig = getLocalConfig();
        const QJsonDocument &localDocument = localConfig.getConfig();
        parseConfig(localDocument);
        emit presetsUpdated();
    }

    void PresetKeywordsModelConfig::saveFromModel(const QVector<Preset> &presets) {
        int size = presets.size();

        m_PresetData.resize(size);
        for (int i = 0; i < size; i++) {
            auto &item = presets[i];
            auto &name = item.name;
            auto &keywordsModel = item.keys;
            auto keywords = keywordsModel->getKeywords();
            m_PresetData[i].keys = keywords;
            m_PresetData[i].name = name;
        }

        writeToConfig();
    }

    bool PresetKeywordsModelConfig::parseConfig(const QJsonDocument &document) {
#ifdef QT_DEBUG
        LOG_DEBUG << document;
#endif
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

    int PresetKeywordsModelConfig::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
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
            for (auto &item: jsonArray) {
                if (!item.isString()) {
                    continue;
                }

                list.append(item.toString());
            }
            m_PresetData.push_back({list, presetKey});
        }
    }

    void PresetKeywordsModelConfig::writeToConfig()
    {
      QJsonArray jsonArray;
      for (auto & item : m_PresetData){
          QJsonObject object;
          QJsonArray keys;
          LOG_WARNING << item.name<< " " << item.keys;
          keys = QJsonArray::fromStringList(item.keys);
          object.insert(item.name, keys);
          jsonArray.append(object);
      }
      QJsonObject topObject;
      topObject.insert(PRESETKEYS_KEY, jsonArray);
      QJsonDocument doc;
      doc.setObject(topObject);
      LOG_WARNING << doc;
      Helpers::LocalConfig &localConfig = getLocalConfig();
      localConfig.setConfig(doc);
      localConfig.saveToFile();
    }
}
