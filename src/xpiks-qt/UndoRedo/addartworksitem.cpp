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
#include "addartworksitem.h"
#include "../Commands/commandmanager.h"
#include "../Models/artitemsmodel.h"
#include "../Common/defines.h"

void UndoRedo::AddArtworksHistoryItem::undo(const Commands::ICommandManager *commandManagerInterface) {
    LOG_INFO << "#";

    Commands::CommandManager *commandManager = (Commands::CommandManager*)commandManagerInterface;

    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
    artItemsModel->removeArtworks(m_AddedRanges);

    commandManager->getDelegator()->saveSessionInBackground();
}
