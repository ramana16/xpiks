/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "removedirectoryitem.h"
#include "../Helpers/filehelpers.h"
#include "../Models/artworksrepository.h"
#include "../Models/filteredartitemsproxymodel.h"
#include "../Commands/commandmanager.h"

void UndoRedo::RemoveDirectoryItem::undo(const Commands::ICommandManager *commandManagerInterface) const {
    Commands::CommandManager *commandManager = (Commands::CommandManager *)commandManagerInterface;
    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();

    if (m_NeedsDeselectionOnUndo) {
        artworksRepository->unselectAllDirectories();
    }

    artworksRepository->insertEmptyDirectory(m_AbsolutePath, m_StartDirectoryIndex, m_IsDirectorySelected);

    RemoveArtworksHistoryItem::undo(commandManagerInterface);

    artworksRepository->updateSelectedState();
    Models::FilteredArtItemsProxyModel *filteredArtItemProxyModel = commandManager->getFilteredArtItemsModel();
    filteredArtItemProxyModel->updateFilter();

    commandManager->saveSessionInBackground();
}
