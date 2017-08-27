/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MOVEIMAGECACHEJOBITEM_H
#define MOVEIMAGECACHEJOBITEM_H

#include "imaintenanceitem.h"

namespace QMLExtensions {
    class ImageCachingService;
}

namespace Maintenance {
    class MoveImageCacheJobItem: public IMaintenanceItem
    {
    public:
        MoveImageCacheJobItem(QMLExtensions::ImageCachingService *imageCachingService);

    public:
        virtual void processJob() override;

    private:
        QMLExtensions::ImageCachingService *m_ImageCachingService;
    };
}

#endif // MOVEIMAGECACHEJOBITEM_H
