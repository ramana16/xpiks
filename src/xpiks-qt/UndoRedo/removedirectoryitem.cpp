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
#include "../Models/settingsmodel.h"

UndoRedo::RemoveDirectoryHistoryItem::RemoveDirectoryHistoryItem(int commandID, int startFileIndex, qint64 dirID, bool wasSelected, bool unselectAll):
    RemoveArtworksHistoryItem(commandID, {}, {}, {}, {wasSelected ? dirID : -1}, unselectAll, true),
    m_DirectoryID(dirID),
    m_StartFileIndex(startFileIndex)
{
}

void UndoRedo::RemoveDirectoryHistoryItem::fillFilesAndVectors(const QString &directoryPath, bool autoFindVectors) {
    LOG_DEBUG << directoryPath << "auto find vectors:" << autoFindVectors;
    QStringList files, vectorFiles, dummy;
    QStringList rawFilenames;

    Helpers::extractFilesFromDirectory(directoryPath, rawFilenames);
    Helpers::splitMediaFiles(rawFilenames, files, dummy);
    vectorFiles.reserve(files.size());

    if (autoFindVectors) {
        for (auto &filepath: files) {
            QStringList vectors = Helpers::convertToVectorFilenames(filepath);

            bool anyFound = false;
            foreach (const QString &item, vectors) {
                if (QFileInfo(item).exists()) {
                    anyFound = true;
                    vectorFiles.append(item);
                    break;
                }
            }

            if (!anyFound) {
                vectorFiles.append("");
            }
        }
    } else {
        int n = files.size();
        while (n--) { vectorFiles.append(""); }
    }

    Q_ASSERT(vectorFiles.size() == files.size());

    QVector<int> indices;
    const int size = files.size();
    indices.reserve(size);
    for (int i = 0; i < size; i++) {
        indices.push_back(m_StartFileIndex + i);
    }

    setRemovedArtworksIndices(indices);
    setRemovedArtworksPathes(files);
    setRemovedAttachedVectors(vectorFiles);
}

void UndoRedo::RemoveDirectoryHistoryItem::undo(const Commands::ICommandManager *commandManagerInterface) {
    Commands::CommandManager *commandManager = (Commands::CommandManager *)commandManagerInterface;
    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();
    auto *xpiks = commandManager->getDelegator();

    QString directoryPath;
    Models::SettingsModel *settingsModel = commandManager->getSettingsModel();
    bool autoFindVectors = settingsModel->getAutoFindVectors();

    if (artworksRepository->tryGetDirectoryPath(m_DirectoryID, directoryPath)) {
        fillFilesAndVectors(directoryPath, autoFindVectors);

        RemoveArtworksHistoryItem::undo(commandManagerInterface);

        artworksRepository->updateSelectedState();
        artworksRepository->refresh();

#ifndef CORE_TESTS
        Models::FilteredArtItemsProxyModel *filteredArtItemProxyModel = commandManager->getFilteredArtItemsModel();
        filteredArtItemProxyModel->updateFilter();
#endif

        xpiks->saveSessionInBackground();
    } else {
        // directory should not be removed until undo stack is empty
        Q_ASSERT(false);
    }
}
