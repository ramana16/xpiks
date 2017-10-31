/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IMAGECACHEREQUEST_H
#define IMAGECACHEREQUEST_H

#include <QString>
#include <QSize>

namespace QMLExtensions {

#define DEFAULT_THUMB_HEIGHT 150
#define DEFAULT_THUMB_WIDTH 150

    class ImageCacheRequest {
    public:
        ImageCacheRequest(const QString &filepath, const QSize &requestedSize, bool recache):
            m_Filepath(filepath),
            m_RequestedSize(requestedSize),
            m_Recache(recache)
        { }

    public:
        const QString &getFilepath() const { return m_Filepath; }
        const QSize &getRequestedSize() const { return m_RequestedSize; }
        bool getNeedRecache() const { return m_Recache; }

    private:
        QString m_Filepath;
        QSize m_RequestedSize;
        bool m_Recache;
    };
}

#endif // IMAGECACHEREQUEST_H
