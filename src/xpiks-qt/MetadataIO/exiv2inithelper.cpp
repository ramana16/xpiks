/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "exiv2inithelper.h"
#include <QtGlobal>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <exiv2/xmp.hpp>

namespace MetadataIO {
    Exiv2InitHelper::Exiv2InitHelper() {
        Exiv2::XmpParser::initialize();
    }

    Exiv2InitHelper::~Exiv2InitHelper() {
        Exiv2::XmpParser::terminate();
    }
}
