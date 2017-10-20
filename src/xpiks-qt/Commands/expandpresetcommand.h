/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EXPANDPRESETCOMMAND_H
#define EXPANDPRESETCOMMAND_H

#include <QVector>
#include <vector>
#include <QStringList>
#include "commandbase.h"
#include "../Models/artworkelement.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class ExpandPresetCommand : public CommandBase
    {
    public:
        ExpandPresetCommand(Models::ArtworkMetadata *artwork,
                            int presetIndex, int keywordIndex = -1):
            CommandBase(CommandType::ExpandPreset),
            m_ArtworkLocker(artwork),
            m_PresetIndex(presetIndex),
            m_KeywordIndex(keywordIndex)
        { }

        virtual ~ExpandPresetCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        Models::ArtworkMetadataLocker m_ArtworkLocker;
        int m_PresetIndex;
        int m_KeywordIndex;
    };

    class ExpandPresetCommandResult : public CommandResult {
    public:
        ExpandPresetCommandResult(int indexToUpdate) :
            m_IndexToUpdate(indexToUpdate)
        {
        }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const override;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        int m_IndexToUpdate;
    };
}

#endif // EXPANDPRESETCOMMAND_H
