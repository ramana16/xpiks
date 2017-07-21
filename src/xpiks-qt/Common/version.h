/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VERSION_H
#define VERSION_H

// About dialog
#define XPIKS_VERSION_SUFFIX release

// update.json (values from 0..9)
#define XPIKS_MAJOR_VERSION 1
#define XPIKS_MINOR_VERSION 4
#define XPIKS_FIX_VERSION 2
#define XPIKS_HOTFIX_VERSION 0

#define VERSION_STR_(x) #x
#define VERSION_STR(x) VERSION_STR_(x)
#define XPIKS_FULL_VERSION_STRING \
    VERSION_STR(XPIKS_MAJOR_VERSION)"."\
    VERSION_STR(XPIKS_MINOR_VERSION)"."\
    VERSION_STR(XPIKS_FIX_VERSION)"."\
    VERSION_STR(XPIKS_HOTFIX_VERSION)

#define XPIKS_VERSION_TO_INT(major, minor, fix) ((major)*100 + (minor)*10 + (fix))
#define XPIKS_FULL_VERSION_TO_INT(major, minor, fix, hotfix) (10*XPIKS_VERSION_TO_INT(major, minor, fix) + (hotfix))

#define XPIKS_FULL_VERSION_INT XPIKS_FULL_VERSION_TO_INT(XPIKS_MAJOR_VERSION, XPIKS_MINOR_VERSION, XPIKS_FIX_VERSION, XPIKS_HOTFIX_VERSION)
#define XPIKS_VERSION_INT XPIKS_VERSION_TO_INT(XPIKS_MAJOR_VERSION, XPIKS_MINOR_VERSION, XPIKS_FIX_VERSION)
#define XPIKS_VERSION_MAJOR_DIVISOR (100)

#ifdef QT_DEBUG
#define XPIKS_VERSION_STRING XPIKS_FULL_VERSION_STRING".dev"
#else
#define XPIKS_VERSION_STRING XPIKS_FULL_VERSION_STRING
#endif

#endif // VERSION_H

