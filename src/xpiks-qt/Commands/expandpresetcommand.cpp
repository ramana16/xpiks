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
#include "../MetadataIO/artworkssnapshot.h"

namespace Commands {
    ExpandPresetCommand::~ExpandPresetCommand() {
    }

    std::shared_ptr<ICommandResult> ExpandPresetCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "Expand preset" << m_PresetID;

        CommandManager *commandManager = (CommandManager*)commandManagerInterface;
        auto *xpiks = commandManager->getDelegator();
        auto *presetsModel = commandManager->getPresetsModel();
        QStringList keywords;

        int indexToUpdate = -1;
        std::vector<UndoRedo::ArtworkMetadataBackup> artworksBackups;
        MetadataIO::WeakArtworksSnapshot affectedArtworks;

        if (presetsModel->tryGetPreset(m_PresetID, keywords)) {
            Models::ArtworkMetadata *artwork = m_ArtworkLocker.getArtworkMetadata();

            indexToUpdate = (int)artwork->getLastKnownIndex();
            artworksBackups.emplace_back(artwork);

            if (m_KeywordIndex != -1) {
                if (artwork->expandPreset(m_KeywordIndex, keywords)) {
                    affectedArtworks.push_back(artwork);
                }
            } else {
                if (artwork->appendKeywords(keywords)) {
                    affectedArtworks.push_back(artwork);
                }
            }
        }

        if (affectedArtworks.size() > 0) {
            xpiks->submitForSpellCheck(affectedArtworks);
            xpiks->submitForWarningsCheck(affectedArtworks);
            xpiks->saveArtworksBackups(affectedArtworks);

            std::unique_ptr<UndoRedo::IHistoryItem> modifyArtworksItem(
                        new UndoRedo::ModifyArtworksHistoryItem(
                            getCommandID(),
                            artworksBackups,
                            QVector<int>() << indexToUpdate,
                            UndoRedo::PasteModificationType));
            xpiks->recordHistoryItem(modifyArtworksItem);
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
