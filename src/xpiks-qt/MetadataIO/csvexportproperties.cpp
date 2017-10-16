/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "csvexportproperties.h"

namespace MetadataIO {
    QLatin1String csvExportPropertyToString(CsvExportPropertyType property){
        switch (property) {
        case Empty: return QLatin1String("Empty");
        case Filename: return QLatin1String("Filename");
        case Title: return QLatin1String("Title");
        case Description: return QLatin1String("Description");
        case Keywords: return QLatin1String("Keywords");
        case Category1: return QLatin1String("Category1");
        case Category2: return QLatin1String("Category2");
        default: return QLatin1String();
        }
    }
}
