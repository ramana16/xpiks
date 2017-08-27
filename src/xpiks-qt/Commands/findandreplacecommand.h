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
    class PreviewMetadataElement;
    class ArtworkMetadata;
}

namespace Commands {
    class FindAndReplaceCommand:
        public CommandBase
    {
    public:
        FindAndReplaceCommand(std::vector<Models::PreviewMetadataElement> &metadataElements,
        const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags):
            CommandBase(CommandType::FindAndReplace),
            m_MetadataElements(std::move(metadataElements)),
            m_ReplaceWhat(replaceWhat),
            m_ReplaceTo(replaceTo),
            m_Flags(flags)
        {}

        virtual ~FindAndReplaceCommand();

    public:
        std::shared_ptr<Commands::ICommandResult> execute(const ICommandManager *commandManagerInterface) const;

    private:
        std::vector<Models::PreviewMetadataElement> m_MetadataElements;
        QString m_ReplaceWhat;
        QString m_ReplaceTo;
        Common::SearchFlags m_Flags;
    };

    class FindAndReplaceCommandResult:
        public CommandResult
    {
    public:
        FindAndReplaceCommandResult(
        const QVector<Models::ArtworkMetadata *> &itemsToSave,
        const QVector<int> &indicesToUpdate):
            m_ItemsToSave(itemsToSave),
            m_IndicesToUpdate(indicesToUpdate)
        {}

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
