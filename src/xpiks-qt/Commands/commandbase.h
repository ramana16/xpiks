/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include "icommandbase.h"

namespace Commands {

    enum struct CommandType {
        AddArtworks = 0,
        RemoveArtworks = 1,
        CombinedEdit = 2,
        PasteKeywords = 3,
        FindAndReplace = 4,
        DeleteKeywords = 5,
        ExpandPreset = 6
    };

    class CommandResult;
    class ICommandManager;

    class CommandBase : public ICommandBase
    {
    public:
        CommandBase(CommandType commandType):
            m_CommandType(commandType),
            m_CommandID(-1)
        {}
        virtual ~CommandBase() {}

    public:
        virtual int getCommandType() const override { return (int)m_CommandType; }
        virtual int getCommandID() const override { return m_CommandID; }
        virtual void assignCommandID(int commandID) override { Q_ASSERT(m_CommandID == -1); m_CommandID = commandID; }

    private:
        CommandType m_CommandType;
        int m_CommandID;
    };

    class CommandResult : public ICommandResult {
    public:
        CommandResult() {}
        virtual ~CommandResult() {}

    public:
        virtual int getStatus() const override { return 0; }
    };
}

#endif // COMMANDBASE_H
