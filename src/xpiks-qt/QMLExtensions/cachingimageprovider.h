/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CACHINGIMAGEPROVIDER_H
#define CACHINGIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QHash>

namespace QMLExtensions {
    class ImageCachingService;

    class CachingImageProvider : public QObject, public QQuickImageProvider
    {
        Q_OBJECT
    public:
        CachingImageProvider(ImageType type, Flags flags = 0) :
            QQuickImageProvider(type, flags),
            m_ImageCachingService(NULL)
        {}

        virtual ~CachingImageProvider() {}

        virtual QImage requestImage(const QString &url, QSize *size, const QSize& requestedSize) override;

    public:
        void setImageCachingService(QMLExtensions::ImageCachingService *cachingService) {
            m_ImageCachingService = cachingService;
        }

    private:
        QMLExtensions::ImageCachingService *m_ImageCachingService;
    };
}

#endif // CACHINGIMAGEPROVIDER_H
