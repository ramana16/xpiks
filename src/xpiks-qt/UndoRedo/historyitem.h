/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include "ihistoryitem.h"

class QString;

namespace Commands {
    class CommandManager;
}

namespace UndoRedo {

    enum struct HistoryActionType {
        AddArtworks,
        RemovedArtworks,
        ModifyArtworks
    };

    class HistoryItem : public IHistoryItem
    {
    public:
        HistoryItem(HistoryActionType actionType, int commandID) :
            m_ActionType(actionType),
            m_CommandID(commandID)
        {}
        virtual ~HistoryItem() {}

    public:
        virtual int getActionType() const override { return (int)m_ActionType; }
        virtual int getCommandID() const override { return m_CommandID; }

    private:
        HistoryActionType m_ActionType;
        int m_CommandID;
    };
}

#endif // HISTORYITEM_H
