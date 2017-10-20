/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FINDANDREPLACECOMMAND_H
#define FINDANDREPLACECOMMAND_H

#include "commandbase.h"
#include <QString>
#include <QVector>
#include "../Common/flags.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class PreviewArtworkElement;
    class ArtworkMetadata;
}

namespace Commands {
    class FindAndReplaceCommand:
        public CommandBase
    {
    public:
        FindAndReplaceCommand(const MetadataIO::ArtworksSnapshot::Container &rawSnapshot,
        const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags):
            CommandBase(CommandType::FindAndReplace),
            m_RawSnapshot(std::move(rawSnapshot)),
            m_ReplaceWhat(replaceWhat),
            m_ReplaceTo(replaceTo),
            m_Flags(flags)
        {}

        virtual ~FindAndReplaceCommand();

    public:
        std::shared_ptr<Commands::ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        MetadataIO::ArtworksSnapshot::Container m_RawSnapshot;
        QString m_ReplaceWhat;
        QString m_ReplaceTo;
        Common::SearchFlags m_Flags;
    };

    class FindAndReplaceCommandResult:
        public CommandResult
    {
    public:
        FindAndReplaceCommandResult(
                MetadataIO::WeakArtworksSnapshot &itemsToSave,
                const QVector<int> &indicesToUpdate):
            m_ItemsToSave(std::move(itemsToSave)),
            m_IndicesToUpdate(indicesToUpdate)
        { }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const override;

#ifndef CORE_TESTS

    private:
#else

    public:
#endif
        MetadataIO::WeakArtworksSnapshot m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // FINDANDREPLACE_H
