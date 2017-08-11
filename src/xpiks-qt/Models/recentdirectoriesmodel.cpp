/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "recentdirectoriesmodel.h"

#define MAX_RECENT_DIRECTORIES 5

namespace Models {
    RecentDirectoriesModel::RecentDirectoriesModel():
        RecentItemsModel(MAX_RECENT_DIRECTORIES)
    {
    }
}

