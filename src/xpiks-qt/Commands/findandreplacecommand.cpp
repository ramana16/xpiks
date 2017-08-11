/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "findandreplacecommand.h"
#include <QObject>
#include "commandmanager.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Models/artworkmetadata.h"
#include "../Models/previewmetadataelement.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"

namespace Commands {
    FindAndReplaceCommand::~FindAndReplaceCommand() { LOG_DEBUG << "#"; }

    std::shared_ptr<Commands::ICommandResult> FindAndReplaceCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "Replacing [" << m_ReplaceWhat << "] to [" << m_ReplaceTo << "] in" << m_MetadataElements.size() << "item(s)";
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;

        QVector<int> indicesToUpdate;
        QVector<Models::ArtworkMetadata *> itemsToSave;

        size_t size = m_MetadataElements.size();
        itemsToSave.reserve((int)size);
        indicesToUpdate.reserve((int)size);

        for (size_t i = 0; i < size; i++) {
            const Models::PreviewMetadataElement &element = m_MetadataElements.at(i);
            if (!element.isSelected()) {
                continue;
            }

            Models::ArtworkMetadata *metadata = element.getOrigin();
            int index = element.getOriginalIndex();

            artworksBackups.emplace_back(metadata);

            bool succeeded = metadata->replace(m_ReplaceWhat, m_ReplaceTo, m_Flags);
            if (succeeded) {
                LOG_FOR_TESTS << "Succeeded";
                itemsToSave.append(metadata);
                indicesToUpdate.append(index);
            } else {
                LOG_INFO << "Failed to replace [" << m_ReplaceWhat << "] to [" << m_ReplaceTo << "] in" << metadata->getFilepath();
            }
        }

        if (indicesToUpdate.size() != 0) {
            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(
                        new UndoRedo::ModifyArtworksHistoryItem(
                            getCommandID(),
                            artworksBackups, indicesToUpdate,
                            UndoRedo::CombinedEditModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        }

        std::shared_ptr<ICommandResult> result(new FindAndReplaceCommandResult(itemsToSave, indicesToUpdate));
        return result;
    }

    void FindAndReplaceCommandResult::afterExecCallback(const Commands::ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        if (!m_IndicesToUpdate.isEmpty()) {
            commandManager->updateArtworks(m_IndicesToUpdate);
        }

        if (!m_ItemsToSave.isEmpty()) {
            commandManager->saveArtworksBackups(m_ItemsToSave);
            commandManager->submitForSpellCheck(m_ItemsToSave);
            commandManager->submitForWarningsCheck(m_ItemsToSave);
        }
    }
}
