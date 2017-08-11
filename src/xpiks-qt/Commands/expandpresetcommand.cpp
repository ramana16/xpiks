/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "expandpresetcommand.h"
#include "commandmanager.h"
#include "../KeywordsPresets/presetkeywordsmodel.h"
#include "../UndoRedo/modifyartworkshistoryitem.h"
#include "../Models/artitemsmodel.h"

namespace Commands {
    ExpandPresetCommand::~ExpandPresetCommand() {
    }

    std::shared_ptr<ICommandResult> ExpandPresetCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "Expand preset" << m_PresetIndex;

        CommandManager *commandManager = (CommandManager*)commandManagerInterface;
        auto *presetsModel = commandManager->getPresetsModel();
        QStringList keywords;

        int indexToUpdate = -1;
        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        QVector<Models::ArtworkMetadata*> affectedArtworks;

        if (presetsModel->tryGetPreset(m_PresetIndex, keywords)) {
            Models::ArtworkMetadata *metadata = m_MetadataElement.getOrigin();

            indexToUpdate = m_MetadataElement.getOriginalIndex();
            artworksBackups.emplace_back(metadata);

            if (m_KeywordIndex != -1) {
                if (metadata->expandPreset(m_KeywordIndex, keywords)) {
                    affectedArtworks.append(metadata);
                }
            } else {
                if (metadata->appendKeywords(keywords)) {
                    affectedArtworks.append(metadata);
                }
            }
        }

        if (affectedArtworks.size() > 0) {
            commandManager->submitForSpellCheck(affectedArtworks);
            commandManager->submitForWarningsCheck(affectedArtworks);
            commandManager->saveArtworksBackups(affectedArtworks);

            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(
                        new UndoRedo::ModifyArtworksHistoryItem(
                            getCommandID(),
                            artworksBackups,
                            QVector<int>() << indexToUpdate,
                            UndoRedo::PasteModificationType));
            commandManager->recordHistoryItem(modifyArtworksItem);
        } else {
            LOG_WARNING << "Failed to expand preset";
        }

        std::shared_ptr<ExpandPresetCommandResult> result(new ExpandPresetCommandResult(indexToUpdate));
        return result;
    }

    void ExpandPresetCommandResult::afterExecCallback(const ICommandManager *commandManagerInterface) const {
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;
        Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
        artItemsModel->updateItems(QVector<int>() << m_IndexToUpdate,
                                   QVector<int>() <<
                                   Models::ArtItemsModel::IsModifiedRole <<
                                   Models::ArtItemsModel::KeywordsCountRole);
    }
}
