/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
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
    const char PATH_TO_EXIFTOOL[] = "PATH_TO_EXIF_TOOL";
    const char PATH_TO_CURL[] = "PATH_TO_CURL";
    const char ORGANIZATION_NAME[] = "Xpiks";
    const char ORGANIZATION_DOMAIN[] = "ribtoks.github.io";
    const char APPLICATION_NAME[] = "Xpiks";
    const char LOG_FILENAME[] = "xpiks.log";
    const char METADATA_BACKUP_EXTENSION[] = ".xpks";

#ifdef QT_NO_DEBUG
    const char UPLOAD_HOSTS[] = "UPLOAD_HOSTS_HASH";
    const char MAX_KEYWORD_COUNT[] = "MAX_KEYWORD_COUNT";
    const char MAX_DESCRIPTION_LENGTH[] = "MAX_DESCRIPTION_LENGTH";
    const char MIN_MEGAPIXEL_COUNT[] = "MIN_MEGAPIXEL_COUNT";
    const char USE_MASTER_PASSWORD[] = "USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "MASTER_PASSWORD_HASH";
    const char ONE_UPLOAD_MINUTES_TIMEMOUT[] = "ONE_UPLOAD_MINUTES_TIMEMOUT";
    const char USE_CONFIRMATION_DIALOGS[] = "USE_CONFIRMATION_DIALOGS";
#else
    const char UPLOAD_HOSTS[] = "DEBUG_UPLOAD_HOSTS_HASH";
    const char MAX_KEYWORD_COUNT[] = "DEBUG_MAX_KEYWORD_COUNT";
    const char MAX_DESCRIPTION_LENGTH[] = "DEBUG_MAX_DESCRIPTION_LENGTH";
    const char MIN_MEGAPIXEL_COUNT[] = "DEBUG_MIN_MEGAPIXEL_COUNT";
    const char USE_MASTER_PASSWORD[] = "DEBUG_USE_MASTER_PASSWORD";
    const char MASTER_PASSWORD_HASH[] = "DEBUG_MASTER_PASSWORD_HASH";
    const char ONE_UPLOAD_MINUTES_TIMEMOUT[] = "DEBUG_ONE_UPLOAD_MINUTES_TIMEMOUT";
    const char USE_CONFIRMATION_DIALOGS[] = "DEBUG_USE_CONFIRMATION_DIALOGS";
#endif // QT_NO_DEBUG
}

#endif // CONSTANTS

