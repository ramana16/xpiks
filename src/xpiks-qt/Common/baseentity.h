/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BASEENTITY
#define BASEENTITY

namespace Commands { class CommandManager; }

#include <QtGlobal>

namespace Common {
    class BaseEntity {
    public:
        BaseEntity() :
            m_CommandManager(0)
        {}

    public:
        virtual void setCommandManager(Commands::CommandManager *commandManager) {
            Q_ASSERT(commandManager != 0);
            m_CommandManager = commandManager;
        }

    protected:
        Commands::CommandManager *m_CommandManager;
    };
}

#endif // BASEENTITY

