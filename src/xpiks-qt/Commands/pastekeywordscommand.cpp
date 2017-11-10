/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QList>
#include "pastekeywordscommand.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../Models/artworkelement.h"
#include "../Commands/commandmanager.h"
#include "../Common/defines.h"
#include "../Models/artitemsmodel.h"

Commands::PasteKeywordsCommand::~PasteKeywordsCommand() {
    LOG_DEBUG << "#";
}

std::shared_ptr<Commands::ICommandResult> Commands::PasteKeywordsCommand::execute(const ICommandManager *commandManagerInterface) const {
    LOG_INFO << "Pasting" << m_KeywordsList.length() << "keywords to" << m_RawSnapshot.size() << "item(s)";

    CommandManager *commandManager = (CommandManager*)commandManagerInterface;

    QVector<int> indicesToUpdate;
    std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
    MetadataIO::WeakArtworksSnapshot affectedArtworks;
    const size_t size = m_RawSnapshot.size();
    indicesToUpdate.reserve((int)size);
    artworksBackups.reserve(size);
    affectedArtworks.reserve((int)size);

    for (size_t i = 0; i < size; ++i) {
        auto &item = m_RawSnapshot.at(i);
        Models::ArtworkMetadata *artwork = item->getArtworkMetadata();

        indicesToUpdate.append((int)artwork->getLastKnownIndex());
        artworksBackups.emplace_back(artwork);

        artwork->appendKeywords(m_KeywordsList);
        affectedArtworks.push_back(artwork);
    }

    if (size > 0) {
        commandManager->submitForSpellCheck(affectedArtworks);
        commandManager->submitForWarningsCheck(affectedArtworks);
        commandManager->saveArtworksBackups(affectedArtworks);

        std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(
                    new UndoRedo::ModifyArtworksHistoryItem(
                        getCommandID(),
                        artworksBackups, indicesToUpdate,
                        UndoRedo::PasteModificationType));
        commandManager->recordHistoryItem(modifyArtworksItem);
    } else {
        LOG_WARNING << "Pasted zero real words!";
    }

    std::shared_ptr<PasteKeywordsCommandResult> result(new PasteKeywordsCommandResult(indicesToUpdate));
    return result;
}

void Commands::PasteKeywordsCommandResult::afterExecCallback(const Commands::ICommandManager *commandManagerInterface) const {
    CommandManager *commandManager = (CommandManager*)commandManagerInterface;
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    artItemsModel->updateItems(m_IndicesToUpdate,
                               QVector<int>() <<
                               Models::ArtItemsModel::IsModifiedRole <<
                               Models::ArtItemsModel::KeywordsCountRole);
}
