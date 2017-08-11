/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IFLAGSPROVIDER_H
#define IFLAGSPROVIDER_H

#include "flags.h"

namespace Common {
    class IFlagsProvider {
    public:
        virtual ~IFlagsProvider() {}

        virtual Common::flag_t getFlags() const = 0;
    };
}

#endif // IFLAGSPROVIDER_H
