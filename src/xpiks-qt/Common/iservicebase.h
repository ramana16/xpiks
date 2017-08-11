/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ISERVICEBASE_H
#define ISERVICEBASE_H

#include <QVector>
#include <memory>
#include "../Common/flags.h"

namespace Common {
    class ServiceStartParams {
    public:
        virtual ~ServiceStartParams() {}
    };

    template<typename T, typename FlagsType=Common::flag_t>
    class IServiceBase {
    public:
        virtual ~IServiceBase() {}

        virtual void startService(const std::shared_ptr<ServiceStartParams> &params) = 0;
        virtual void stopService() = 0;

        // if service is provided via plugin it can be turned off
        virtual bool isAvailable() const = 0;

        virtual bool isBusy() const = 0;

        virtual void submitItem(T *item) = 0;
        virtual void submitItem(T *item, FlagsType flags) = 0;
        virtual void submitItems(const QVector<T*> &items) = 0;
    };
}

#endif // ISERVICEBASE_H
