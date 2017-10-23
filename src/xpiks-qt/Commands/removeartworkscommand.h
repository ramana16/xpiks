/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef REMOVEARTWORKSCOMMAND_H
#define REMOVEARTWORKSCOMMAND_H

#include <QVector>
#include <QPair>
#include "commandbase.h"
#include "../Helpers/indiceshelper.h"

namespace Commands {
    class RemoveArtworksCommand : public CommandBase
    {
    public:
        RemoveArtworksCommand(const QVector<QPair<int, int> > &rangesToRemove, bool removeAsDirectory):
            CommandBase(CommandType::RemoveArtworks),
            m_RangesToRemove(rangesToRemove),
            m_RemoveAsDirectory(removeAsDirectory)
        {
        }

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        QVector<QPair<int, int> > m_RangesToRemove;
        bool m_RemoveAsDirectory;
    };

    class RemoveArtworksCommandResult : public CommandResult {
    public:
        RemoveArtworksCommandResult(int removedCount):
            m_RemovedArtworksCount(removedCount)
        {}
    public:
        int m_RemovedArtworksCount;
    };
}

#endif // REMOVEARTWORKSCOMMAND_H
