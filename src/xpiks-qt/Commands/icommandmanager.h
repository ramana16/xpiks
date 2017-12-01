/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ICOMMANDMANAGER_H
#define ICOMMANDMANAGER_H

#include <memory>

#include "../Common/iservicebase.h"
#include "../Common/ibasicartwork.h"
#include "../Common/flags.h"

namespace Commands {
    class ICommandBase;
    class ICommandResult;

    class ICommandManager {
    public:
        virtual ~ICommandManager() {}

        virtual std::shared_ptr<Commands::ICommandResult> processCommand(const std::shared_ptr<ICommandBase> &command) = 0;
    };
}

#endif // ICOMMANDMANAGER_H
