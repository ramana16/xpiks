/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IHISTORYITEM_H
#define IHISTORYITEM_H

#include <QString>

namespace Commands {
    class ICommandManager;
}

namespace UndoRedo {

    class IHistoryItem {
    public:
        virtual ~IHistoryItem() {}

        virtual void undo(const Commands::ICommandManager *commandManager) const = 0;
        virtual QString getDescription() const = 0;
        virtual int getActionType() const = 0;
        virtual int getCommandID() const = 0;
    };
}

#endif // IHISTORYITEM_H
