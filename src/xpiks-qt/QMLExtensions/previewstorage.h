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

#ifndef PREVIEWSTORAGE_H
#define PREVIEWSTORAGE_H

#include <QString>
#include "cachedimage.h"

namespace QMLExtensions {
    template<class TKey, class TValue>
    class PreviewStorage {
    public:
        virtual ~PreviewStorage() {}

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;
        virtual void sync() = 0;

    public:
        virtual void update(const TKey &key, TValue &value) = 0;
        virtual bool tryGet(const TKey &key, /*out*/ TValue &value) = 0;
    };
}

#endif // PREVIEWSTORAGE_H
