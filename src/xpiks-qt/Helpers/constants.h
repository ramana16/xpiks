/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CONSTANTS
#define CONSTANTS

namespace Constants {
    // common for DEBUG and RELEASE
    const char PATH_TO_EXIFTOOL[] = "PATH_TO_EXIF_TOOL";
    const char ORGANIZATION_NAME[] = "Xpiks";
    const char ORGANIZATION_DOMAIN[] = "ribtoks.github.io";
    const char APPLICATION_NAME[] = "Xpiks";
    const char LOG_FILENAME[] = "xpiks.log";
    const char LOG_PREFIX[] = "xpiks-";
    const char METADATA_BACKUP_EXTENSION[] = ".xpks";
    const char METADATA_BACKUP_SUFFIX[] = "xpks";
    const char SAVE_BACKUPS[] = "SAVE_BACKUPS";
    const char KEYWORD_SIZE_SCALE[] = "KEYWORD_SIZE_SCALE";
    const char DISMISS_DURATION[] = "DISMISS_DURATION";
    const char FIT_SMALL_PREVIEW[] = "FIT_SMALL_PREVIEW";
    const char SEARCH_USING_AND[] = "SEARCH_USING_AND";
    const char SEARCH_BY_FILEPATH[] = "SEARCH_BY_FILEPATH";
    const char WHATS_NEW_FILENAME[] = "whatsnew.txt";
    const char TERMS_AND_CONDITIONS_FILENAME[] = "terms_and_conditions.txt";
    const char DICT_PATH[] = "DICT_PATH";
    const char USER_STATISTICS[] = "USER_STATISTICS";
    const char CHECK_FOR_UPDATES[] = "CHECK_FOR_UPDATES";
    const char NUMBER_OF_LAUNCHES[] = "NUMBER_OF_LAUNCHES";
    const char APP_WINDOW_WIDTH []  = "WINDOW_SIZE_X";
    const char APP_WINDOW_HEIGHT [] = "WINDOW_SIZE_Y";
    const char APP_WINDOW_X[] = "WINDOW_POSITION_X";
    const char APP_WINDOW_Y[] = "WINDOW_POSITION_Y";
    const char AUTO_FIND_VECTORS[] = "AUTO_FIND_VECTORS";
    const char USE_PROXY[] = "USE_PROXY";
    const char PROXY_HOST[] = "PROXY_HOST_HASH";
    const char UPDATES_DIRECTORY[] = "updates";
    const char FAILED_PLUGINS_DIR[] = "invalid";
    const char IMAGE_CACHE_TABLE[] = "imgcache";
    const char VIDEO_CACHE_TABLE[] = "vidcache";
    const char METADATA_CACHE_TABLE[] = "metadatacache";

    // different for DEBUG and RELEASE

#ifdef QT_NO_DEBUG
    const char UPLOAD_HOSTS[] = "UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "MASTER_PASSWORD_HASH";
    const char ONE_UPLOAD_SECONDS_TIMEMOUT[] = "ONE_UPLOAD_SECONDS_TIMEMOUT";
    const char USE_CONFIRMATION_DIALOGS[] = "USE_CONFIRMATION_DIALOGS";
    const char RECENT_DIRECTORIES[] = "RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "RECENT_FILES";
    const char MAX_PARALLEL_UPLOADS[] = "MAX_PARALLEL_UPLOADS";
    const char USE_SPELL_CHECK[] = "USE_SPELL_CHECK";
    const char USER_AGENT_ID[] = "USER_AGENT_ID";
    const char INSTALLED_VERSION[] = "INSTALLED_VERSION";
    const char USER_CONSENT[] = "USER_CONSENT_1_0";
    const char SELECTED_LOCALE[] = "SELECTED_LOCALE";
    const char SELECTED_THEME_INDEX[] = "SELECTED_THEME_INDEX";
    const char USE_AUTO_COMPLETE[] = "USE_AUTO_COMPLETE";
    const char USE_EXIFTOOL[] = "USE_EXIFTOOL";
    const char IMAGES_CACHE_DIR[] = "imagescache";
    const char IMAGES_CACHE_INDEX[] = "imagescache.index";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "CACHE_IMAGES_AUTOMATICALLY";
    const char SCROLL_SPEED_SENSIVITY[] = "SCROLL_SPEED_SENSIVITY";
    const char AUTO_DOWNLOAD_UPDATES[] = "AUTO_DOWNLOAD_UPDATES";
    const char USER_DICT_FILENAME[] = "userdict.dic";
    const char PATH_TO_UPDATE[] = "PATH_TO_UPDATE";
    const char AVAILABLE_UPDATE_VERSION[] = "AVAILABLE_UPDATE_VERSION";
    const char ARTWORK_EDIT_RIGHT_PANE_WIDTH[] = "ARTWORK_EDIT_RIGHT_PANE_WIDTH";
    const char TRANSLATOR_SELECTED_DICT_INDEX[] = "TRANSLATOR_SELECTED_DICT_INDEX";
    const char TRANSLATOR_DIR[] = "dictionaries";
    const char PLUGINS_DIR[] = "XpiksPlugins";
    const char VIDEO_CACHE_DIR[] = "videoscache";
    const char STATES_DIR[] = "states";
    const char DB_DIR[] = "db";
    const char DB_DIR_FAILOVER[] = "db_failover";
    const char METADATA_DB_NAME[] = "metadata.db";
    const char IMAGECACHE_DB_NAME[] = "imgcache.db";
    const char VIDEOCACHE_DB_NAME[] = "videocache.db";
    const char METADATA_CACHE_DB_NAME[] = "metadatacache.db";
    const char LOGS_DIR[] = "logs";
#else
    // common for DEBUG and INTEGRATION_TESTS
    const char ONE_UPLOAD_SECONDS_TIMEMOUT[] = "DEBUG_ONE_UPLOAD_SECONDS_TIMEMOUT";
    const char USE_CONFIRMATION_DIALOGS[] = "DEBUG_USE_CONFIRMATION_DIALOGS";
    const char MAX_PARALLEL_UPLOADS[] = "DEBUG_MAX_PARALLEL_UPLOADS";
    const char USE_SPELL_CHECK[] = "DEBUG_USE_SPELL_CHECK";
    const char USER_AGENT_ID[] = "DEBUG_USER_AGENT_ID";
    const char INSTALLED_VERSION[] = "DEBUG_INSTALLED_VERSION";
    const char USER_CONSENT[] = "DEBUG_USER_CONSENT_1_0";
    const char SELECTED_LOCALE[] = "DEBUG_SELECTED_LOCALE";
    const char SELECTED_THEME_INDEX[] = "DEBUG_SELECTED_THEME_INDEX";
    const char FTP_STOCKS_FILENAME[] = "stocks_ftp.json";
    const char USE_AUTO_COMPLETE[] = "DEBUG_USE_AUTO_COMPLETE";
    const char USE_EXIFTOOL[] = "DEBUG_USE_EXIFTOOL";
    const char IMAGES_CACHE_DIR[] = "debug_imagescache";
    const char IMAGES_CACHE_INDEX[] = "debug_imagescache.index";
    const char SCROLL_SPEED_SENSIVITY[] = "DEBUG_SCROLL_SPEED_SENSIVITY";
    const char AUTO_DOWNLOAD_UPDATES[] = "DEBUG_AUTO_DOWNLOAD_UPDATES";
    const char PATH_TO_UPDATE[] = "DEBUG_PATH_TO_UPDATE";
    const char AVAILABLE_UPDATE_VERSION[] = "DEBUG_AVAILABLE_UPDATE_VERSION";
    const char ARTWORK_EDIT_RIGHT_PANE_WIDTH[] = "DEBUG_ARTWORK_EDIT_RIGHT_PANE_WIDTH";
    const char TRANSLATOR_SELECTED_DICT_INDEX[] = "DEBUG_TRANSLATOR_SELECTED_DICT_INDEX";
    const char TRANSLATOR_DIR[] = "debug_dictionaries";
    const char PLUGINS_DIR[] = "debug_XpiksPlugins";
    const char VIDEO_CACHE_DIR[] = "debug_videoscache";
    const char STATES_DIR[] = "debug_states";

    // different for DEBUG and INTEGRATION_TESTS
#ifdef INTEGRATION_TESTS
    const char UPLOAD_HOSTS[] = "INTEGRATION_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "INTEGRATION_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "INTEGRATION_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "INTEGRATION_RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "INTEGRATION_RECENT_FILES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "INTEGRATION_CACHE_IMAGES_AUTOMATICALLY";
    const char USER_DICT_FILENAME[] = "userdict_debug_tests.dic";
    const char DB_DIR[] = "tests_db";
    const char DB_DIR_FAILOVER[] = "tests_db_failover";
    const char METADATA_DB_NAME[] = "tests_metadata.db";
    const char IMAGECACHE_DB_NAME[] = "tests_imgcache.db";
    const char VIDEOCACHE_DB_NAME[] = "tests_videocache.db";
    const char METADATA_CACHE_DB_NAME[] = "tests_metadatacache.db";
    const char LOGS_DIR[] = "tests_logs";
#else
    const char UPLOAD_HOSTS[] = "DEBUG_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "DEBUG_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "DEBUG_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "DEBUG_RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "DEBUG_RECENT_FILES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "DEBUG_CACHE_IMAGES_AUTOMATICALLY";
    const char USER_DICT_FILENAME[] = "userdict_debug.dic";
    const char DB_DIR[] = "debug_db";
    const char DB_DIR_FAILOVER[] = "debug_db_failover";
    const char METADATA_DB_NAME[] = "debug_metadata.db";
    const char IMAGECACHE_DB_NAME[] = "debug_imgcache.db";
    const char VIDEOCACHE_DB_NAME[] = "debug_videocache.db";
    const char METADATA_CACHE_DB_NAME[] = "debug_metadatacache.db";
    const char LOGS_DIR[] = "debug_logs";
#endif
#endif // QT_NO_DEBUG

    const char settingsVersion[] = "settingsVersion";

    const char pathToExifTool[] = "pathToExifTool";
    const char saveBackups[] = "saveBackups";
    const char keywordSizeScale[] = "keywordSizeScale";
    const char dismissDuration[] = "dismissDuration";
    const char fitSmallPreview[] = "fitSmallPreview";
    const char searchUsingAnd[] = "searchUsingAnd";
    const char searchByFilepath[] = "searchByFilepath";
    const char dictPath[] = "dictPath";
    const char userStatistics[] = "userStatistics";
    const char checkForUpdates[] = "checkForUpdates";
    const char numberOfLaunches[] = "numberOfLaunches";
    const char appWindowWidth []  = "appWindowWidth";
    const char appWindowHeight [] = "appWindowHeight";
    const char appWindowX[] = "appWindowX";
    const char appWindowY[] = "appWindowY";
    const char autoFindVectors[] = "autoFindVectors";
    const char useProxy[] = "useProxy";
    const char proxyHost[] = "proxyHostHash";
    const char legacyUploadHosts[] = "legacyUploadHostsHash";
    const char useMasterPassword[] = "useMasterPassword";
    const char masterPasswordHash[] = "masterPasswordHash";
    const char oneUploadSecondsTimeout[] = "oneUploadSecondsTimeout";
    const char useConfirmationDialogs[] = "useConfirmationDialogs";
    const char recentDirectories[] = "recentDirectories";
    const char recentFiles[] = "recentFiles";
    const char maxParallelUploads[] = "maxParallelUploads";
    const char useSpellCheck[] = "useSpellCheck";
    const char detectDuplicates[] = "detectDuplicates";
    const char userAgentId[] = "userAgentId";
    const char installedVersion[] = "installedVersion";
    const char userConsent[] = "userConsent10";
    const char selectedLocale[] = "selectedLocale";
    const char selectedThemeIndex[] = "selectedThemeIndex";
    const char useKeywordsAutoComplete[] = "useKeywordsAutoComplete";
    const char usePresetsAutoComplete[] = "usePresetsAutoComplete";
    const char useExifTool[] = "useExifTool";
    const char cacheImagesAutomatically[] = "cacheImagesAutomatically";
    const char scrollSpeedSensivity[] = "scrollSpeedSensivity";
    const char autoDownloadUpdates[] = "autoDownloadUpdates";
    const char pathToUpdate[] = "pathToUpdate";
    const char availableUpdateVersion[] = "availableUpdateVersion";
    const char artworkEditRightPaneWidth[] = "artworkEditRightPaneWidth";
    const char translatorSelectedDictIndex[] = "translatorSelectedDictIndex";
    const char verboseUpload[] = "verboseUpload";
    const char saveSession[] = "saveSession";
    const char useProgressiveSuggestionPreviews[] = "useProgressiveSuggestionPreviews";
    const char progressiveSuggestionIncrement[] = "progressiveSuggestionIncrement";
    const char useDirectExiftoolExport[] = "useDirectExiftoolExport";
    const char suggestorSearchTypeIndex[] = "suggestorSearchTypeIndex";
    const char useAutoImport[] = "useAutoImport";
}

#endif // CONSTANTS

