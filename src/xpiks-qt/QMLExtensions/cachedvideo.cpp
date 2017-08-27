/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "cachedvideo.h"

namespace QMLExtensions {
    CachedVideo::CachedVideo():
        m_RequestsServed(0),
        m_IsSmartThumbnail(false)
    {
    }

    CachedVideo::CachedVideo(const CachedVideo &from):
        m_LastModified(from.m_LastModified),
        m_Filename(from.m_Filename),
        m_RequestsServed(from.m_RequestsServed),
        m_IsSmartThumbnail(from.m_IsSmartThumbnail),
        m_AdditionalData(from.m_AdditionalData)
    {
    }

    CachedVideo &CachedVideo::operator=(const CachedVideo &other) {
        m_LastModified = other.m_LastModified;
        m_Filename = other.m_Filename;
        m_RequestsServed = other.m_RequestsServed;
        m_IsSmartThumbnail = other.m_IsSmartThumbnail;
        m_AdditionalData = other.m_AdditionalData;

        return *this;
    }

    QDataStream &operator<<(QDataStream &out, const CachedVideo &v) {
        out << v.m_Filename << v.m_LastModified << v.m_RequestsServed << v.m_IsSmartThumbnail << v.m_AdditionalData;
        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedVideo &v) {
        in >> v.m_Filename >> v.m_LastModified >> v.m_RequestsServed << v.m_IsSmartThumbnail >> v.m_AdditionalData;
        return in;
    }
}
