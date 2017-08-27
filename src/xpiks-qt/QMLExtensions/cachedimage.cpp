/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
