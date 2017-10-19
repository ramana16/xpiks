/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "warningssettingsmodel.h"
#include "../Connectivity/apimanager.h"
#include <QStandardPaths>
#include <QDir>

#define OVERWRITE_WARNINGS_CONFIG false
#define LOCAL_WARNINGS_SETTINGS_FILE QLatin1String("warnings_settings.json")

#define OVERWRITE_KEY QLatin1String("overwrite")
#define SETTINGS_KEY QLatin1String("settings")
#define ALLOWED_CHARACTERS QLatin1String("additional_allowed_chars")
#define MIN_MEGAPIXELS QLatin1String("min_megapixels")
#define MAX_IMAGE_FILESIZE_MB QLatin1String("max_image_filesize_mb")
#define MAX_VIDEO_FILESIZE_MB QLatin1String("max_video_filesize_mb")
#define MIN_KEYWORDS_COUNT QLatin1String("min_keywords_count")
#define MAX_KEYWORDS_COUNT QLatin1String("max_keywords_count")
#define MIN_WORDS_COUNT QLatin1String("min_words_count")
#define MAX_DESCRIPTION_LENGTH QLatin1String("max_description_length")
#define MAX_VIDEO_DURATION QLatin1String("max_video_duration_s")
#define MIN_VIDEO_DURATION QLatin1String("min_video_duration_s")

#define DEFAULT_MIN_MEGAPIXELS 4.0
#define DEFAULT_MAX_IMAGE_FILESIZE_MB 25.0
#define DEFAULT_MAX_VIDEO_FILESIZE_MB 4096.0
#define DEFAULT_MIN_KEYWORDS_COUNT 7
#define DEFAULT_MAX_KEYWORDS_COUNT 50
#define DEFAULT_MIN_WORDS_COUNT 3
#define DEFAULT_MAX_DESCRIPTION_LENGTH 200
#define DEFAULT_MIN_VIDEO_DURATION_SECONDS 5
#define DEFAULT_MAX_VIDEO_DURATION_SECONDS 60

namespace Warnings {
    WarningsSettingsModel::WarningsSettingsModel():
        Models::AbstractConfigUpdaterModel(OVERWRITE_WARNINGS_CONFIG),
        m_AllowedFilenameCharacters("._-@#"),
        m_MinMegapixels(DEFAULT_MIN_MEGAPIXELS),
        m_MaxImageFilesizeMB(DEFAULT_MAX_IMAGE_FILESIZE_MB),
        m_MaxVideoFilesizeMB(DEFAULT_MAX_VIDEO_FILESIZE_MB),
        m_MinVideoDurationSeconds(DEFAULT_MIN_VIDEO_DURATION_SECONDS),
        m_MaxVideoDurationSeconds(DEFAULT_MAX_VIDEO_DURATION_SECONDS),
        m_MinKeywordsCount(DEFAULT_MIN_KEYWORDS_COUNT),
        m_MaxKeywordsCount(DEFAULT_MAX_KEYWORDS_COUNT),
        m_MinWordsCount(DEFAULT_MIN_WORDS_COUNT),
        m_MaxDescriptionLength(DEFAULT_MAX_DESCRIPTION_LENGTH)
    {}

    void WarningsSettingsModel::initializeConfigs() {
        LOG_DEBUG << "#";
        QString localConfigPath;

        QString appDataPath = XPIKS_USERDATA_PATH;

        if (!appDataPath.isEmpty()) {
            QDir appDataDir(appDataPath);
            localConfigPath = appDataDir.filePath(LOCAL_WARNINGS_SETTINGS_FILE);
        } else {
            localConfigPath = LOCAL_WARNINGS_SETTINGS_FILE;
        }

        auto &apiManager = Connectivity::ApiManager::getInstance();
        QString remoteAddress = apiManager.getWarningSettingsAddr();
        AbstractConfigUpdaterModel::initializeConfigs(remoteAddress, localConfigPath);
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

    bool WarningsSettingsModel::processLocalConfig(const QJsonDocument &document) {
        LOG_INTEGR_TESTS_OR_DEBUG << document;
        bool result = parseConfig(document);
        return result;
    }

    void WarningsSettingsModel::processMergedConfig(const QJsonDocument &document) {
        LOG_DEBUG << "#";
        parseConfig(document);
    }

    bool WarningsSettingsModel::parseConfig(const QJsonDocument &document) {
        LOG_DEBUG << "#";
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

            {
                QJsonValue allowedCharacters = settingsObject[ALLOWED_CHARACTERS];
                if (!allowedCharacters.isString()) {
                    LOG_WARNING << "ALLOWED_CHARACTERS value is not string";
                    anyError = true;
                }

                m_AllowedFilenameCharacters = allowedCharacters.toString();
            }

            {
                QJsonValue minMPixels = settingsObject[MIN_MEGAPIXELS];
                if (!minMPixels.isDouble()) {
                    LOG_WARNING << "MIN_MEGAPIXELS value is not number";
                    anyError = true;
                }

                m_MinMegapixels = minMPixels.toDouble(DEFAULT_MIN_MEGAPIXELS);
            }

            {
                QJsonValue maxImageFilesizeMB = settingsObject[MAX_IMAGE_FILESIZE_MB];
                if (!maxImageFilesizeMB.isDouble()) {
                    LOG_WARNING << "MAX_IMAGE_FILESIZE_MB value is not number";
                    anyError = true;
                }

                m_MaxImageFilesizeMB = maxImageFilesizeMB.toDouble(DEFAULT_MAX_IMAGE_FILESIZE_MB);
            }

            {
                QJsonValue maxVideoFilesizeMB = settingsObject[MAX_VIDEO_FILESIZE_MB];
                if (!maxVideoFilesizeMB.isDouble()) {
                    LOG_WARNING << "MAX_VIDEO_FILESIZE_MB value is not number";
                    anyError = true;
                }

                m_MaxVideoFilesizeMB = maxVideoFilesizeMB.toDouble(DEFAULT_MAX_VIDEO_FILESIZE_MB);
            }

            {
                QJsonValue minVideoDuration = settingsObject[MIN_VIDEO_DURATION];
                if (!minVideoDuration.isDouble()) {
                    LOG_WARNING << "MAX_VIDEO_DURATION value is not number";
                    anyError = true;
                }

                m_MinVideoDurationSeconds = minVideoDuration.toDouble(DEFAULT_MIN_VIDEO_DURATION_SECONDS);
            }

            {
                QJsonValue maxVideoDuration = settingsObject[MAX_VIDEO_DURATION];
                if (!maxVideoDuration.isDouble()) {
                    LOG_WARNING << "MAX_VIDEO_DURATION value is not number";
                    anyError = true;
                }

                m_MaxVideoDurationSeconds = maxVideoDuration.toDouble(DEFAULT_MAX_VIDEO_DURATION_SECONDS);
            }

            {
                QJsonValue minKeywordsCount = settingsObject[MIN_KEYWORDS_COUNT];
                if (!minKeywordsCount.isDouble()) {
                    LOG_WARNING << "MIN_KEYWORDS_COUNT value is not number";
                    anyError = true;
                }

                m_MinKeywordsCount = minKeywordsCount.toInt(DEFAULT_MIN_KEYWORDS_COUNT);
            }

            {
                QJsonValue maxKeywordsCount = settingsObject[MAX_KEYWORDS_COUNT];
                if (!maxKeywordsCount.isDouble()) {
                    LOG_WARNING << "MAX_KEYWORDS_COUNT value is not number";
                    anyError = true;
                }

                m_MaxKeywordsCount = maxKeywordsCount.toInt(DEFAULT_MAX_KEYWORDS_COUNT);
            }

            {
                QJsonValue minWordsCount = settingsObject[MIN_WORDS_COUNT];
                if (!minWordsCount.isDouble()) {
                    LOG_WARNING << "MIN_WORDS_COUNT value is not number";
                    anyError = true;
                }

                m_MinWordsCount = minWordsCount.toInt(DEFAULT_MIN_WORDS_COUNT);
            }

            {
                QJsonValue maxDescriptionCount = settingsObject[MAX_DESCRIPTION_LENGTH];
                if (!maxDescriptionCount.isDouble()) {
                    LOG_WARNING << "MAX_DESCRIPTION_LENGTH value is not number";
                    anyError = true;
                }

                m_MaxDescriptionLength = maxDescriptionCount.toInt(DEFAULT_MAX_DESCRIPTION_LENGTH);
            }
        } while (false);

        return anyError;
    }

    int WarningsSettingsModel::operator ()(const QJsonObject &val1, const QJsonObject &val2) {
        Q_UNUSED(val1);
        Q_UNUSED(val2);
        return 0;
    }
}
