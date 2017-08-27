/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "deletekeywordscommand.h"
#include "../Commands/commandmanager.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"

namespace Commands {
    DeleteKeywordsCommand::DeleteKeywordsCommand(std::vector<Models::MetadataElement> &infos,
                                                 const QSet<QString> &keywordsSet, bool caseSensitive):
        CommandBase(CommandType::DeleteKeywords),
        m_MetadataElements(std::move(infos)),
        m_KeywordsSet(keywordsSet),
        m_CaseSensitive(caseSensitive)
    {
    }

    std::shared_ptr<ICommandResult> DeleteKeywordsCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << m_KeywordsSet.size() << "keyword(s) to remove from" << m_MetadataElements.size() << "item(s)";
        LOG_INFO << "Case sensitive:" << m_CaseSensitive;
        QVector<int> indicesToUpdate;
        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        MetadataIO::WeakArtworksSnapshot affectedItems;

        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        size_t size = m_MetadataElements.size();
        indicesToUpdate.reserve((int)size);
        artworksBackups.reserve(size);
        affectedItems.reserve((int)size);

        for (size_t i = 0; i < size; ++i) {
            const Models::MetadataElement &info = m_MetadataElements.at(i);
            Models::ArtworkMetadata *metadata = info.getOrigin();

            artworksBackups.emplace_back(metadata);

            if (metadata->removeKeywords(m_KeywordsSet, m_CaseSensitive)) {
                indicesToUpdate.append(info.getOriginalIndex());
                affectedItems.append(metadata);
            } else {
                artworksBackups.pop_back();
            }
        }

        if (!artworksBackups.empty()) {
            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(
                        new UndoRedo::ModifyArtworksHistoryItem(
                            getCommandID(),
                            artworksBackups, indicesToUpdate,
                            UndoRedo::CombinedEditModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        }

        std::shared_ptr<ICommandResult> result(new DeleteKeywordsCommandResult(affectedItems, indicesToUpdate));
        return result;
    }

    void DeleteKeywordsCommandResult::afterExecCallback(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        if (!m_IndicesToUpdate.isEmpty()) {
            commandManager->updateArtworksAtIndices(m_IndicesToUpdate);
        }

        if (!m_AffectedItems.isEmpty()) {
            commandManager->saveArtworksBackups(m_AffectedItems);
            commandManager->submitForSpellCheck(m_AffectedItems);
            commandManager->submitForWarningsCheck(m_AffectedItems);
        }
    }
}
