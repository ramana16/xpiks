/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef COMBINEDEDITCOMMAND_H
#define COMBINEDEDITCOMMAND_H

#include <QStringList>
#include <QString>
#include <vector>
#include <QVector>
#include "commandbase.h"
#include "../Common/flags.h"
#include "../MetadataIO/artworkssnapshot.h"

namespace Models {
    class ArtworkMetadata;
}

namespace Commands {

    class CombinedEditCommand: public CommandBase
    {
    public:
        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            MetadataIO::ArtworksSnapshot::Container &rawSnapshot,
                            const QString &description, const QString &title,
                            const QStringList &keywords) :
            CommandBase(CommandType::CombinedEdit),
            m_RawSnapshot(std::move(rawSnapshot)),
            m_ArtworkDescription(description),
            m_ArtworkTitle(title),
            m_Keywords(keywords),
            m_EditFlags(editFlags)
        { }

        CombinedEditCommand(Common::CombinedEditFlags editFlags,
                            MetadataIO::ArtworksSnapshot::Container &rawSnapshot) :
            CommandBase(CommandType::CombinedEdit),
            m_RawSnapshot(std::move(rawSnapshot)),
            m_EditFlags(editFlags)
        { }

        virtual ~CombinedEditCommand();

    public:
        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManagerInterface) const override;

    private:
        void setKeywords(Models::ArtworkMetadata *metadata) const;
        void setDescription(Models::ArtworkMetadata *metadata) const;
        void setTitle(Models::ArtworkMetadata *metadata) const;

    private:
        MetadataIO::ArtworksSnapshot::Container m_RawSnapshot;
        QString m_ArtworkDescription;
        QString m_ArtworkTitle;
        QStringList m_Keywords;
        Common::CombinedEditFlags m_EditFlags;
    };

    class CombinedEditCommandResult : public CommandResult {
    public:
        CombinedEditCommandResult(MetadataIO::WeakArtworksSnapshot &affectedItems,
                                  MetadataIO::WeakArtworksSnapshot &itemsToSave,
                                  const QVector<int> &indicesToUpdate) :
            m_AffectedItems(std::move(affectedItems)),
            m_ItemsToSave(std::move(itemsToSave)),
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
        MetadataIO::WeakArtworksSnapshot m_ItemsToSave;
        QVector<int> m_IndicesToUpdate;
    };
}

#endif // COMBINEDEDITCOMMAND_H
