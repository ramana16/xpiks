/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

    // different for DEBUG and RELEASE

#ifdef QT_NO_DEBUG
    const char UPLOAD_HOSTS[] = "UPLOAD_HOSTS_HASH";
    const char MAX_KEYWORD_COUNT[] = "MAX_KEYWORD_COUNT";
    const char MAX_DESCRIPTION_LENGTH[] = "MAX_DESCRIPTION_LENGTH";
    const char MIN_MEGAPIXEL_COUNT[] = "MIN_MEGAPIXEL_COUNT";
    const char USE_MASTER_PASSWORD[] = "USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "MASTER_PASSWORD_HASH";
    const char ONE_UPLOAD_SECONDS_TIMEMOUT[] = "ONE_UPLOAD_SECONDS_TIMEMOUT";
    const char USE_CONFIRMATION_DIALOGS[] = "USE_CONFIRMATION_DIALOGS";
    const char RECENT_DIRECTORIES[] = "RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "RECENT_FILES";
    const char MAX_PARALLEL_UPLOADS[] = "MAX_PARALLEL_UPLOADS";
    const char USE_SPELL_CHECK[] = "USE_SPELL_CHECK";
    const char LIBRARY_FILENAME[] = "xpiks.v14.library";
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
#else
    const char MAX_KEYWORD_COUNT[] = "DEBUG_MAX_KEYWORD_COUNT";
    const char MAX_DESCRIPTION_LENGTH[] = "DEBUG_MAX_DESCRIPTION_LENGTH";
    const char MIN_MEGAPIXEL_COUNT[] = "DEBUG_MIN_MEGAPIXEL_COUNT";
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

#ifdef INTEGRATION_TESTS
    const char LIBRARY_FILENAME[] = "xpiks.integration.v14.library";
    const char UPLOAD_HOSTS[] = "INTEGRATION_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "INTEGRATION_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "INTEGRATION_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "INTEGRATION_RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "INTEGRATION_RECENT_FILES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "INTEGRATION_CACHE_IMAGES_AUTOMATICALLY";
    const char USER_DICT_FILENAME[] = "userdict_debug_tests.dic";
#else
    const char LIBRARY_FILENAME[] = "xpiks.debug.v14.library";
    const char UPLOAD_HOSTS[] = "DEBUG_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "DEBUG_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "DEBUG_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "DEBUG_RECENT_DIRECTORIES";
    const char RECENT_FILES[] = "DEBUG_RECENT_FILES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "DEBUG_CACHE_IMAGES_AUTOMATICALLY";
    const char USER_DICT_FILENAME[] = "userdict_debug.dic";
#endif
#endif // QT_NO_DEBUG

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
    const char uploadHosts[] = "uploadHostsHash";
    const char maxKeywordCount[] = "maxKeywordCount";
    const char maxDescriptionLength[] = "maxDescriptionLength";
    const char minMegapixelCount[] = "minMegapixelCount";
    const char useMasterPassword[] = "useMasterPassword";
    const char masterPasswordHash[] = "masterPasswordHash";
    const char oneUploadSecondsTimeout[] = "oneUploadSecondsTimeout";
    const char useConfirmationDialogs[] = "useConfirmationDialogs";
    const char recentDirectories[] = "recentDirectories";
    const char recentFiles[] = "recentFiles";
    const char maxParallelUploads[] = "maxParallelUploads";
    const char useSpellCheck[] = "useSpellCheck";
    const char userAgentId[] = "userAgentId";
    const char installedVersion[] = "installedVersion";
    const char userConsent[] = "userConsent10";
    const char selectedLocale[] = "selectedLocale";
    const char selectedThemeIndex[] = "selectedThemeIndex";
    const char useAutoComplete[] = "useAutoComplete";
    const char useExifTool[] = "useExifTool";
    const char cacheImagesAutomatically[] = "cacheImagesAutomatically";
    const char scrollSpeedSensivity[] = "scrollSpeedSensivity";
    const char autoDownloadUpdates[] = "autoDownloadUpdates";
    const char pathToUpdate[] = "pathToUpdate";
    const char availableUpdateVersion[] = "availableUpdateVersion";
    const char artworkEditRightPaneWidth[] = "artworkEditRightPaneWidth";
    const char translatorSelectedDictIndex[] = "translatorSelectedDictIndex";
    const char verboseUpload[] = "verboseUpload";
}

#endif // CONSTANTS

