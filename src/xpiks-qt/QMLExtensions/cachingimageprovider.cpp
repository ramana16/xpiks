/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cachingimageprovider.h"
#include "../Common/defines.h"
#include "../QMLExtensions/imagecachingservice.h"

#define RECACHE true

namespace QMLExtensions {
    QImage CachingImageProvider::requestImage(const QString &url, QSize *size, const QSize &requestedSize) {
        Q_ASSERT(!url.isEmpty());
        if (url.isEmpty()) { return QImage(); }

        QString id;

        if (url.contains(QChar('%'))) {
            QUrl initialUrl(url);
            id = initialUrl.path();
        } else {
            id = url;
        }

        QString cachedPath;
        bool needsUpdate = false;

        if (m_ImageCachingService->tryGetCachedImage(id, requestedSize, cachedPath, needsUpdate)) {
            QImage image(cachedPath);
            *size = image.size();

            if (needsUpdate) {
                LOG_INFO << "Recaching image" << id;
                m_ImageCachingService->cacheImage(id, requestedSize, RECACHE);
            }

            return image;
        } else {
            LOG_INTEGR_TESTS_OR_DEBUG << "Not found cached:" << id;

            QImage image(id);
            QImage result;

            if (requestedSize.isValid()) {
                m_ImageCachingService->cacheImage(id, requestedSize);
                result = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                LOG_WARNING << "Size is invalid:" << requestedSize.width() << "x" << requestedSize.height();
                m_ImageCachingService->cacheImage(id);
                result = image;
            }

            *size = result.size();
            return result;
        }
    }
}
