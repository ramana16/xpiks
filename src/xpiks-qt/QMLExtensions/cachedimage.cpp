/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cachedimage.h"

namespace QMLExtensions {
    CachedImage::CachedImage():
        m_RequestsServed(0)
    {
    }

    CachedImage::CachedImage(const CachedImage &from):
        m_LastModified(from.m_LastModified),
        m_Filename(from.m_Filename),
        m_Size(from.m_Size),
        m_RequestsServed(from.m_RequestsServed),
        m_AdditionalData(from.m_AdditionalData)
    {
    }

    CachedImage &CachedImage::operator=(const CachedImage &other) {
        m_LastModified = other.m_LastModified;
        m_Filename = other.m_Filename;
        m_Size = other.m_Size;
        m_RequestsServed = other.m_RequestsServed;
        m_AdditionalData = other.m_AdditionalData;

        return *this;
    }

    QDataStream &operator<<(QDataStream &out, const CachedImage &v) {
        out << v.m_Filename << v.m_LastModified << v.m_Size << v.m_RequestsServed << v.m_AdditionalData;
        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedImage &v) {
        in >> v.m_Filename >> v.m_LastModified >> v.m_Size >> v.m_RequestsServed >> v.m_AdditionalData;
        return in;
    }
}
