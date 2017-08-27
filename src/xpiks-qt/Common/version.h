/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

#define XPIKS_API_VERSION_STRING \
    VERSION_STR(XPIKS_MAJOR_VERSION)\
    VERSION_STR(XPIKS_MINOR_VERSION)\
    VERSION_STR(XPIKS_FIX_VERSION)

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

