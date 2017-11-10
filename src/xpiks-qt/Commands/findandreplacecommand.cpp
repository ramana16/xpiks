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
#include "../Models/previewartworkelement.h"
#include "../UndoRedo/artworkmetadatabackup.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Common/defines.h"
#include "../Helpers/filterhelpers.h"

namespace Commands {
    FindAndReplaceCommand::~FindAndReplaceCommand() { LOG_DEBUG << "#"; }

    std::shared_ptr<Commands::ICommandResult> FindAndReplaceCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "Replacing [" << m_ReplaceWhat << "] to [" << m_ReplaceTo << "] in" << m_RawSnapshot.size() << "item(s)";
        CommandManager *commandManager = (CommandManager *)commandManagerInterface;

        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;

        QVector<int> indicesToUpdate;
        MetadataIO::WeakArtworksSnapshot itemsToSave;

        size_t size = m_RawSnapshot.size();
        itemsToSave.reserve(size);
        indicesToUpdate.reserve((int)size);

        for (auto &locker: m_RawSnapshot) {
            std::shared_ptr<Models::ArtworkElement> element = std::dynamic_pointer_cast<Models::ArtworkElement>(locker);
            Q_ASSERT(element);
            if (!element->getIsSelected()) { continue; }

            Models::ArtworkMetadata *artwork = locker->getArtworkMetadata();

            artworksBackups.emplace_back(artwork);

            bool succeeded = artwork->replace(m_ReplaceWhat, m_ReplaceTo, m_Flags);
            if (succeeded) {
                LOG_FOR_TESTS << "Succeeded";
                itemsToSave.push_back(artwork);
                indicesToUpdate.append((int)artwork->getLastKnownIndex());
            } else {
                LOG_INFO << "Failed to replace [" << m_ReplaceWhat << "] to [" << m_ReplaceTo << "] in" << artwork->getFilepath();
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
            commandManager->updateArtworksAtIndices(m_IndicesToUpdate);
        }

        if (!m_ItemsToSave.empty()) {
            commandManager->saveArtworksBackups(m_ItemsToSave);
            commandManager->submitForSpellCheck(m_ItemsToSave);
            commandManager->submitForWarningsCheck(m_ItemsToSave);
        }
    }
}
