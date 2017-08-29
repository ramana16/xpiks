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
#include "../Common/version.h"

namespace QMLExtensions {
    CachedVideo::CachedVideo():
        m_Version(0),
        m_RequestsServed(0),
        m_IsQuickThumbnail(false)
    {
        if (XPIKS_MAJOR_VERSION_CHECK(1, 5) ||
                XPIKS_MAJOR_VERSION_CHECK(1, 4)) {
            m_Version = 1;
        }
    }

    CachedVideo::CachedVideo(const CachedVideo &from):
        m_Version(from.m_Version),
        m_LastModified(from.m_LastModified),
        m_Filename(from.m_Filename),
        m_RequestsServed(from.m_RequestsServed),
        m_IsQuickThumbnail(from.m_IsQuickThumbnail)
    {
    }

    CachedVideo &CachedVideo::operator=(const CachedVideo &other) {
        m_Version = other.m_Version;
        m_LastModified = other.m_LastModified;
        m_Filename = other.m_Filename;
        m_RequestsServed = other.m_RequestsServed;
        m_IsQuickThumbnail = other.m_IsQuickThumbnail;

        return *this;
    }

    QDataStream &operator<<(QDataStream &out, const CachedVideo &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(!XPIKS_VERSION_CHECK(1, 5, 0));
#ifndef TRAVIS_CI
        out.setVersion(QDataStream::Qt_5_6);
#endif

        out << v.m_Version;
        out << v.m_Filename;
        out << v.m_LastModified;
        out << v.m_RequestsServed;
        out << v.m_IsQuickThumbnail;

        Q_ASSERT(out.status() == QDataStream::Ok);

        return out;
    }

    QDataStream &operator>>(QDataStream &in, CachedVideo &v) {
        // TODO: update before release to Qt 5.9
        Q_ASSERT(!XPIKS_VERSION_CHECK(1, 5, 0));
#ifndef TRAVIS_CI
        in.setVersion(QDataStream::Qt_5_6);
#endif

        in >> v.m_Version;
        in >> v.m_Filename;
        in >> v.m_LastModified;
        in >> v.m_RequestsServed;
        in >> v.m_IsQuickThumbnail;

        Q_ASSERT(in.status() == QDataStream::Ok);

        return in;
    }
}
