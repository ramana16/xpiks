#include "warningssettingsmodel.h"
#include <QStandardPaths>
#include <QDir>

#define OVERWRITE_WARNINGS_CONFIG false
#define WARNINGS_SETTINGS_URL "https://ribtoks.github.io/xpiks/api/v1/warnings_settings.json"
#define LOCAL_WARNINGS_SETTINGS_FILE QLatin1String("warnings_settings.json")

#define OVERWRITE_KEY QLatin1String("overwrite")
#define SETTINGS_KEY QLatin1String("settings")
#define ALLOWED_CHARACTERS QLatin1String("chars")
#define MIN_MPIXELS QLatin1String("min_mpixels")
#define MAX_KEYWORDS_COUNT QLatin1String("max_keywords_count")
#define MAX_DESCRIPTION_LENGTH QLatin1String("max_description_length")

namespace  AutoComplete {
    WarningsSettingsModel::WarningsSettingsModel():
        Models::AbstractConfigUpdaterModel(OVERWRITE_WARNINGS_CONFIG),
        m_AllowedFilenameCharacters(".,_-@ "),
        m_MinMegapixels(4),
        m_MaxKeywordsCount(50),
        m_MaxDescriptionLength(200)
    {}

    void WarningsSettingsModel::initializeConfigs() {
        LOG_DEBUG <<'#';
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;

        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_WARNINGS_SETTINGS_FILE);
        } else {
            localConfigPath = LOCAL_WARNINGS_SETTINGS_FILE;
        }

        AbstractConfigUpdaterModel::initializeConfigs(WARNINGS_SETTINGS_URL, localConfigPath);

        const Helpers::LocalConfig &localConfig = getLocalConfig();
        const QJsonDocument &localDocument = localConfig.getConfig();
        parseConfig(localDocument);
    }

    void WarningsSettingsModel::processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal) {
        bool overwrite = false;

        if (!overwriteLocal && remoteDocument.isObject()) {
            QJsonObject rootObject = remoteDocument.object();
            if (rootObject.contains(OVERWRITE_KEY)) {
                QJsonValue overwriteValue = rootObject[OVERWRITE_KEY];
                if (overwriteValue.isBool()) {
                    overwrite = overwriteValue.toBool();
                    LOG_DEBUG << "Overwrite flag present in the config:" << overwrite;
                } else {
                    LOG_WARNING << "Overwrite flag is not boolean";
                }
            }
        } else {
            overwrite = overwriteLocal;
        }

        Models::AbstractConfigUpdaterModel::processRemoteConfig(remoteDocument, overwrite);
    }

    bool WarningsSettingsModel::parseConfig(const QJsonDocument &document) {
        LOG_DEBUG<<document;
        bool anyError = false;

        do {
            if (!document.isObject()) {
                LOG_WARNING << "Json document is not an object";
                anyError = true;
                break;
            }

            QJsonObject rootObject = document.object();
            if (!rootObject.contains(SETTINGS_KEY)) {
                LOG_WARNING << "There's no settings key in json";
                anyError = true;
                break;
            }

            QJsonValue settingsValue = rootObject[SETTINGS_KEY];
            if (!settingsValue.isObject()) {
                LOG_WARNING << "Settings object is not an object";
                anyError = true;
                break;
            }

            QJsonObject settingsObject = settingsValue.toObject();

            QJsonValue allowedCharacters = settingsObject[ALLOWED_CHARACTERS];
            if (!allowedCharacters.isString()) {
                LOG_WARNING << "ALLOWED_CHARACTERS value is not string";
                anyError = true;
                break;
            }

            m_AllowedFilenameCharacters = allowedCharacters.toString();

            QJsonValue minMPixels = settingsObject[MIN_MPIXELS];
            if (!minMPixels.isDouble()) {
                LOG_WARNING << "MIN_MPIXELS value is not number";
                anyError = true;
                break;
            }

            m_MinMegapixels = minMPixels.toDouble();

            QJsonValue maxKeywordsCount = settingsObject[MAX_KEYWORDS_COUNT];
            if (!maxKeywordsCount.isDouble()) {
                LOG_WARNING << "MAX_KEYWORDS_COUNT value is not number";
                anyError = true;
                break;
            }

            m_MaxKeywordsCount = maxKeywordsCount.toDouble();

            QJsonValue maxDescriptionCount = settingsObject[MAX_DESCRIPTION_LENGTH];
            if (!maxDescriptionCount.isDouble()) {
                LOG_WARNING << "MAX_DESCRIPTION_LENGTH value is not number";
                anyError = true;
                break;
            }

            m_MaxDescriptionLength = maxDescriptionCount.toDouble();
        } while (false);

        LOG_DEBUG<<anyError;

        return anyError;
    }

    int WarningsSettingsModel::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
    }
}
