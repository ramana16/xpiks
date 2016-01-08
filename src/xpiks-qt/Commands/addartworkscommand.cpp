/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>
#include <QFileInfo>
#include <QVector>
#include "addartworkscommand.h"
#include "commandmanager.h"
#include "../Models/artworksrepository.h"
#include "../Models/artworkmetadata.h"
#include "../Models/artitemsmodel.h"
#include "../UndoRedo/addartworksitem.h"
#include "../Common/defines.h"

Commands::CommandResult *Commands::AddArtworksCommand::execute(const CommandManager *commandManager) const
{
    qDebug() << "Add artworks command:" << m_FilePathes.length() << "files";
    Models::ArtworksRepository *artworksRepository = commandManager->getArtworksRepository();
    Models::ArtItemsModel *artItemsModel = commandManager->getArtItemsModel();

    const int newFilesCount = artworksRepository->getNewFilesCount(m_FilePathes);
    const int initialCount = artItemsModel->rowCount();
    bool filesWereAccounted = artworksRepository->beginAccountingFiles(m_FilePathes);

    QVector<Models::ArtworkMetadata*> artworksToImport;
    artworksToImport.reserve(newFilesCount);

    if (newFilesCount > 0) {
        qInfo() << "Add artworks command:" << newFilesCount << "new files";
        artItemsModel->beginAccountingFiles(newFilesCount);

        int count = m_FilePathes.count();
        artworksToImport.reserve(count);

        for (int i = 0; i < count; ++i) {
            const QString &filename = m_FilePathes[i];

            if (artworksRepository->accountFile(filename))
            {
                Models::ArtworkMetadata *metadata = artItemsModel->createMetadata(filename);
                commandManager->connectArtworkSignals(metadata);

                artItemsModel->appendMetadata(metadata);
                artworksToImport.append(metadata);

                const QString &dirPath = QFileInfo(filename).absolutePath();
                commandManager->addToRecentDirectories(dirPath);
            }
        }

        artItemsModel->endAccountingFiles();
    }

    artworksRepository->endAccountingFiles(filesWereAccounted);

    if (newFilesCount > 0) {
        commandManager->readMetadata(artworksToImport);
        artworksRepository->updateCountsForExistingDirectories();
        artItemsModel->raiseArtworksAdded(newFilesCount);

        UndoRedo::AddArtworksHistoryItem *addArtworksItem = new UndoRedo::AddArtworksHistoryItem(initialCount, newFilesCount);
        commandManager->recordHistoryItem(addArtworksItem);
    }

    AddArtworksCommandResult *result = new AddArtworksCommandResult(newFilesCount);
    return result;
}
