/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "moveimagecachejobitem.h"
#include "../QMLExtensions/imagecachingservice.h"
#include "../Common/defines.h"

namespace Maintenance {
    MoveImageCacheJobItem::MoveImageCacheJobItem(QMLExtensions::ImageCachingService *imageCachingService):
        m_ImageCachingService(imageCachingService)
    {
        Q_ASSERT(imageCachingService != nullptr);
    }

    void MoveImageCacheJobItem::processJob() {
        LOG_DEBUG << "#";
#ifndef CORE_TESTS
        m_ImageCachingService->upgradeCacheStorage();
#endif
    }
}
