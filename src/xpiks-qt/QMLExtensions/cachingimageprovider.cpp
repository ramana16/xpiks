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
    QString prepareUrl(const QString &url) {
        QString id;

        if (url.contains(QChar('%'))) {
            QUrl initialUrl(url);
            id = initialUrl.path();
        } else {
            id = url;
        }

        return id;
    }

    QImage CachingImageProvider::requestImage(const QString &url, QSize *size, const QSize &requestedSize) {
        Q_ASSERT(!url.isEmpty());
        if (url.isEmpty()) { return QImage(); }

        const QString id = prepareUrl(url);

        QString cachedPath;
        bool needsUpdate = false;

        if (m_ImageCachingService->tryGetCachedImage(id, requestedSize, cachedPath, needsUpdate)) {
            QImage cachedImage(cachedPath);
            *size = cachedImage.size();

            if (needsUpdate) {
                LOG_INFO << "Recaching image" << id;
                m_ImageCachingService->cacheImage(id, requestedSize, RECACHE);
            }

            if (!cachedImage.isNull()) {
                return cachedImage;
            }
        }

        LOG_INTEGR_TESTS_OR_DEBUG << "Not found properly cached:" << id;

        QImage originalImage(id);
        *size = originalImage.size();

        QImage result;

        if (requestedSize.isValid()) {
            m_ImageCachingService->cacheImage(id, requestedSize);
            result = originalImage.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            LOG_WARNING << "Size is invalid:" << requestedSize.width() << "x" << requestedSize.height();
            m_ImageCachingService->cacheImage(id);
            result = originalImage.scaled(m_ImageCachingService->getDefaultSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        return result;
    }
}
