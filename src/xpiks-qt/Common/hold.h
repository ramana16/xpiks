/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HOLD_H
#define HOLD_H

#include <QAtomicInt>

namespace Common {
    class Hold {
    public:
        Hold() : m_RefCount(1)
        {}

        ~Hold() {
        }

    public:
        virtual void acquire() { int prev = m_RefCount.fetchAndAddOrdered(1); Q_ASSERT(prev > 0); }
        virtual bool release() { int prev = m_RefCount.fetchAndSubOrdered(1); Q_ASSERT(prev >= 0); return prev == 1; }
        virtual int get() { return m_RefCount.load(); }

    private:
        QAtomicInt m_RefCount;
    };

    class FakeHold: public Hold {
    public:
        virtual void acquire() { Q_ASSERT(false); }
        virtual bool release() { Q_ASSERT(false); return false; }
    };
}

#endif // HOLD_H
