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
#include "../Common/version.h"

namespace QMLExtensions {
    CachedImage::CachedImage():
        m_Version(0),
        m_RequestsServed(0)
    {
        if (XPIKS_MAJOR_VERSION_CHECK(1, 5) ||
                XPIKS_MAJOR_VERSION_CHECK(1, 4)) {
            m_Version = 1;
        }
    }

    CachedImage::CachedImage(const CachedImage &from):
        m_Version(from.m_Version),
        m_LastModified(from.m_LastModified),
        m_Filename(from.m_Filename),
        m_Size(from.m_Size),
        m_RequestsServed(from.m_RequestsServed)
    {
    }

    CachedImage &CachedImage::operator=(const CachedImage &other) {
        m_Version = other.m_Version;
        m_LastModified = other.m_LastModified;
        m_Filename = other.m_Filename;
        m_Size = other.m_Size;
        m_RequestsServed = other.m_RequestsServed;

        return *this;
    }

    QDataStream &operator<<(QDataStream &out, const CachedImage &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(XPIKS_VERSION_CHECK(1, 5, 0));
        out.setVersion(QDataStream::Qt_5_6);

        out << v.m_Version;
        out << v.m_Filename;
        out << v.m_LastModified;
        out << v.m_Size;
        out << v.m_RequestsServed;

        Q_ASSERT(out.status() == QDataStream::Ok);

        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedImage &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(XPIKS_VERSION_CHECK(1, 5, 0));
        in.setVersion(QDataStream::Qt_5_6);

        in >> v.m_Version;
        in >> v.m_Filename;
        in >> v.m_LastModified;
        in >> v.m_Size;
        in >> v.m_RequestsServed;

        Q_ASSERT(in.status() == QDataStream::Ok);

        return in;
    }
}
