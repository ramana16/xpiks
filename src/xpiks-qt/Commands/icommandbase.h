/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ICOMMANDBASE_H
#define ICOMMANDBASE_H

#include <memory>
#include <QtGlobal>

namespace Commands {
    class ICommandManager;

    class ICommandResult {
    public:
        virtual ~ICommandResult() {}
        virtual void afterExecCallback(const ICommandManager *commandManager) const { Q_UNUSED(commandManager); }
        virtual int getStatus() const = 0;
    };

    class ICommandBase {
    public:
        virtual ~ICommandBase() {}

        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManager) const = 0;
        virtual int getCommandType() const = 0;
        virtual int getCommandID() const = 0;
        virtual void assignCommandID(int commandID) = 0;
    };
}

#endif // ICOMMANDBASE_H
