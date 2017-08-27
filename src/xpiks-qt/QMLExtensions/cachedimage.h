/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CACHEDIMAGE_H
#define CACHEDIMAGE_H

#include <QString>
#include <QHash>
#include <QSize>
#include <QDateTime>
#include <QDataStream>

namespace QMLExtensions {
    struct CachedImage {
        CachedImage();
        CachedImage(const CachedImage &from);
        CachedImage &operator=(const CachedImage &other);

        QDateTime m_LastModified;
        QString m_Filename;
        QSize m_Size;
        quint64 m_RequestsServed;
        // reserved for future demands
        QHash<qint32, QByteArray> m_AdditionalData;
    };

    QDataStream &operator<<(QDataStream &out, const CachedImage &v);
    QDataStream &operator>>(QDataStream &in, CachedImage &v);
}

#endif // CACHEDIMAGE_H
