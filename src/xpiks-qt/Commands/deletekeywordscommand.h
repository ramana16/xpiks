/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef DELETEKEYWORDSCOMMAND_H
#define DELETEKEYWORDSCOMMAND_H

#include "commandbase.h"
#include <QStringList>
#include <QSet>
#include <QString>
#include <vector>
#include "../Models/artworkelement.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {
    class DeleteKeywordsCommand : public CommandBase
    {
    public:
        DeleteKeywordsCommand(MetadataIO::ArtworksSnapshot::Container &rawSnapshot,
                              const QSet<QString> &keywordsSet,
                              bool caseSensitive);
        virtual ~DeleteKeywordsCommand() {}

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        MetadataIO::ArtworksSnapshot::Container m_RawSnapshot;
        QSet<QString> m_KeywordsSet;
        bool m_CaseSensitive;
    };

    class DeleteKeywordsCommandResult : public CommandResult {
    public:
        DeleteKeywordsCommandResult(MetadataIO::WeakArtworksSnapshot &affectedItems,
                                    const QVector<int> &indicesToUpdate):
            m_AffectedItems(std::move(affectedItems)),
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
        MetadataIO::WeakArtworksSnapshot m_AffectedItems;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // DELETEKEYWORDSCOMMAND_H
