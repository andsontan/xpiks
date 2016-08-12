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

#ifndef VERSION_H
#define VERSION_H

// About dialog
#define XPIKS_VERSION_SUFFIX release

// update.json (values from 0..9)
#define XPIKS_MAJOR_VERSION 1
#define XPIKS_MINOR_VERSION 3
#define XPIKS_FIX_VERSION 4

#define XPIKS_VERSION_INT (XPIKS_MAJOR_VERSION*100 + XPIKS_MINOR_VERSION*10 + XPIKS_FIX_VERSION)

#ifdef QT_DEBUG
#define XPIKS_VERSION_STRING "1.3.4.dev"
#else
#define XPIKS_VERSION_STRING "1.3.4"
#endif

#endif // VERSION_H

