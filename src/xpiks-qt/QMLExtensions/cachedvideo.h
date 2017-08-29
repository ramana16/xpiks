/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CACHEDVIDEO_H
#define CACHEDVIDEO_H

#include <QDateTime>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QDataStream>

namespace QMLExtensions {
    struct CachedVideo {
        CachedVideo();
        CachedVideo(const CachedVideo &from);
        CachedVideo &operator=(const CachedVideo &other);

        quint32 m_Version; // 32-bit for allignment
        // BEGIN of data version 1
        QDateTime m_LastModified;
        QString m_Filename;
        quint64 m_RequestsServed;
        // quick vs good quality thumbnail
        bool m_IsQuickThumbnail;
        // END of data version 1
    };

    QDataStream &operator<<(QDataStream &out, const CachedVideo &v);
    QDataStream &operator>>(QDataStream &in, CachedVideo &v);
}

#endif // CACHEDVIDEO_H
