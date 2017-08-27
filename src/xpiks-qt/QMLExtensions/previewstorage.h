/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
