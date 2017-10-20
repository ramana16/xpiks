/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PASTEKEYWORDSCOMMAND_H
#define PASTEKEYWORDSCOMMAND_H

#include <QVector>
#include <vector>
#include <QStringList>
#include "commandbase.h"
#include "../Models/artworkelement.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class PasteKeywordsCommand : public CommandBase
    {
    public:
        PasteKeywordsCommand(MetadataIO::ArtworksSnapshot::Container &rawSnapshot,
                             const QStringList &keywords) :
            CommandBase(CommandType::PasteKeywords),
            m_RawSnapshot(std::move(rawSnapshot)),
            m_KeywordsList(keywords)
        {}

        virtual ~PasteKeywordsCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        MetadataIO::ArtworksSnapshot::Container m_RawSnapshot;
        QStringList m_KeywordsList;
    };

    class PasteKeywordsCommandResult : public CommandResult {
    public:
        PasteKeywordsCommandResult(const QVector<int> &indicesToUpdate) :
            m_IndicesToUpdate(indicesToUpdate)
        {
        }

    public:
        virtual void afterExecCallback(const ICommandManager *commandManagerInterface) const override;

#ifndef CORE_TESTS
    private:
#else
    public:
#endif
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // PASTEKEYWORDSCOMMAND_H
