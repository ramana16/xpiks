/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QVector>
#include <QPair>
#include <QString>
#include "removeartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artitemsmodel.h"
#include "../Models/artworkmetadata.h"
#include "../Helpers/indiceshelper.h"
#include "../UndoRedo/removeartworksitem.h"
#include "../UndoRedo/removedirectoryitem.h"
#include "../Common/defines.h"
#include "../Models/imageartwork.h"

namespace Commands {
    std::shared_ptr<ICommandResult> RemoveArtworksCommand::execute(const ICommandManager *commandManagerInterface) const {
        LOG_INFO << "removing" << m_RangesToRemove.length() << "ranges received";
        CommandManager *commandManager = (CommandManager*)commandManagerInterface;

        Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();
        QSet<qint64> touchedDirectories;

        const int count = m_RangesToRemove.count();

        QVector<int> removedItemsIndices;
        removedItemsIndices.reserve(count);

        QStringList removedItemsFilepathes;
        QStringList removedAttachedVectors;
        removedItemsFilepathes.reserve(count);
        removedAttachedVectors.reserve(count);

        for (int k = 0; k < count; ++k) {
            const QPair<int, int> &item = m_RangesToRemove[k];
            int first = item.first;
            int last = item.second;

            for (int i = first; i <= last; ++i) {
                Models::ArtworkMetadata *artwork = artItemsModel->getArtwork(i);
                if (artwork != NULL) {
                    removedItemsIndices.append(i);

                    if (!artwork->isUnavailable()) {
                        const QString &filepath = artwork->getFilepath();
                        removedItemsFilepathes.append(filepath);
                        touchedDirectories.insert(artwork->getDirectoryID());

                        Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(artwork);

                        if (image != NULL && image->hasVectorAttached()) {
                            removedAttachedVectors.append(image->getAttachedVectorPath());
                        } else {
                            removedAttachedVectors.append("");
                        }
                    }
                }
            }
        }

        int artworksToRemoveCount = removedItemsIndices.count();

        LOG_INFO << "removing" << artworksToRemoveCount << "real items found";

        if (artworksToRemoveCount > 0) {
            QVector<QPair<int, int> > rangesToRemove;
            Helpers::indicesToRanges(removedItemsIndices, rangesToRemove);
            artItemsModel->removeItemsAtIndices(rangesToRemove);

            commandManager->clearCurrentItem();

            Models::ArtworksRepository *artworkRepository = commandManager->getArtworksRepository();
            artworkRepository->refresh();
            const auto beforeSelectedCount = artworkRepository->retrieveSelectedDirsCount();
            const auto removedSelectedDirectoryIds = artworkRepository->consolidateSelectionForEmpty();
            const auto afterSelectedCount = artworkRepository->retrieveSelectedDirsCount();
            bool unselectAll = (afterSelectedCount + removedSelectedDirectoryIds.size()) != beforeSelectedCount;
            artworkRepository->unwatchFilePaths(removedItemsFilepathes);

            QStringList notEmptyVectors = removedAttachedVectors;
            notEmptyVectors.removeAll("");
            artworkRepository->unwatchFilePaths(notEmptyVectors);

            artItemsModel->updateModifiedCount();

            if (!m_RemoveAsDirectory) {
                LOG_DEBUG << "removing files one by one";
                if (!removedItemsFilepathes.empty()) {
                    std::unique_ptr<UndoRedo::IHistoryItem> removeArtworksItem(
                            new UndoRedo::RemoveArtworksHistoryItem(getCommandID(),
                                                                    removedItemsIndices,
                                                                    removedItemsFilepathes,
                                                                    removedAttachedVectors,
                                                                    removedSelectedDirectoryIds,
                                                                    unselectAll));
                    commandManager->recordHistoryItem(removeArtworksItem);
                }
            } else {
                LOG_DEBUG << "Removing files as directory";
                Q_ASSERT(touchedDirectories.size() == 1);
                if (touchedDirectories.size() == 1) {
                    auto itBegin = touchedDirectories.begin();
                    qint64 dirID = *itBegin;
                    int firstArtworkIndex = removedItemsIndices.first();
                    const bool wasSelected = removedSelectedDirectoryIds.contains(dirID);
                    std::unique_ptr<UndoRedo::IHistoryItem> removeDirectoryItem(
                                new UndoRedo::RemoveDirectoryHistoryItem(getCommandID(), firstArtworkIndex, dirID, wasSelected, unselectAll));
                    commandManager->recordHistoryItem(removeDirectoryItem);
                }
            }

            commandManager->saveSessionInBackground();

        } else {
            LOG_WARNING << "No items to remove found!";
        }

        // TODO: to be filled with useful return data in future
        std::shared_ptr<ICommandResult> result(new RemoveArtworksCommandResult(artworksToRemoveCount));
        return result;
    }
}
