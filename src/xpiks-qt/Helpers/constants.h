/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
    const char SAVE_BACKUPS[] = "SAVE_BACKUPS";
    const char KEYWORD_SIZE_SCALE[] = "KEYWORD_SIZE_SCALE";
    const char DISMISS_DURATION[] = "DISMISS_DURATION";
    const char FIT_SMALL_PREVIEW[] = "FIT_SMALL_PREVIEW";
    const char SEARCH_USING_AND[] = "SEARCH_USING_AND";
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
    const char USER_DICT_FILENAME[] = "userdict.dic";

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
    const char MAX_PARALLEL_UPLOADS[] = "MAX_PARALLEL_UPLOADS";
    const char USE_SPELL_CHECK[] = "USE_SPELL_CHECK";
    const char LIBRARY_FILENAME[] = "xpiks.library";
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

#ifdef INTEGRATION_TESTS
    const char LIBRARY_FILENAME[] = "xpiks.integration.library";
    const char UPLOAD_HOSTS[] = "INTEGRATION_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "INTEGRATION_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "INTEGRATION_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "INTEGRATION_RECENT_DIRECTORIES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "INTEGRATION_CACHE_IMAGES_AUTOMATICALLY";
#else
    const char LIBRARY_FILENAME[] = "xpiks.debug.library";
    const char UPLOAD_HOSTS[] = "DEBUG_UPLOAD_HOSTS_HASH";
    const char USE_MASTER_PASSWORD[] = "DEBUG_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "DEBUG_MASTER_PASSWORD_HASH";
    const char RECENT_DIRECTORIES[] = "DEBUG_RECENT_DIRECTORIES";
    const char CACHE_IMAGES_AUTOMATICALLY[] = "DEBUG_CACHE_IMAGES_AUTOMATICALLY";
#endif
#endif // QT_NO_DEBUG
}

#endif // CONSTANTS

