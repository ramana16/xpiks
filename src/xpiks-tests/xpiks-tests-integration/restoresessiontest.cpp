/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/sessionmanager.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/MetadataIO/artworkmetadatasnapshot.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "restoresessiontest.h"
#include "signalwaiter.h"
#include "testshelpers.h"

QString RestoreSessionTest::testName() {
    return QLatin1String("RestoreSessionTest");
}

void RestoreSessionTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();

    settingsModel->setUseSpellCheck(false);
    settingsModel->setSaveSession(true);
    settingsModel->setAutoFindVectors(true);
}

int RestoreSessionTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    Models::SessionManager *sessionManager = m_CommandManager->getSessionManager();
    VERIFY(sessionManager->itemsCount() == 0, "Session is not cleared");
    Models::ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();

    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/pixmap/img_0007.jpg")
          << getFilePathForTest("images-for-tests/pixmap/seagull-for-clear.jpg")
          << getFilePathForTest("images-for-tests/pixmap/seagull.jpg")
          << getFilePathForTest("images-for-tests/vector/026.jpg")
          << getFilePathForTest("images-for-tests/vector/027.jpg")
          << getFilePathForTest("images-for-tests/mixed/0267.jpg");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    int addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == files.length(), "Failed to add file");
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }
    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    sleepWait(10, [&]() {
        return sessionManager->itemsCount() == files.length();
    });
    VERIFY(sessionManager->itemsCount() == files.length(), "Session does not contain all files");

    MetadataIO::ArtworksSnapshot oldArtworksSnapshot(artItemsModel->getArtworkList());

    artworksRepository->resetEverything();
    artItemsModel->fakeDeleteAllItems();
    LOG_DEBUG << "About to restore...";

    int restoredCount = m_CommandManager->restoreSessionForTest();
    VERIFY(addedCount == restoredCount, "Failed to properly restore");
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading session metadata.");
    }
    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    MetadataIO::ArtworksSnapshot newArtworksSnapshot(artItemsModel->getArtworkList());
    auto &oldArtworksList = oldArtworksSnapshot.getSnapshot();
    auto &newArtworksList = newArtworksSnapshot.getSnapshot();

    VERIFY(oldArtworksList.size() == newArtworksList.size(), "Old and new snapshots have different number of items");
    LOG_INFO << "Comparing" << oldArtworksList.size() << "items";

    for (size_t i = 0; i < newArtworksList.size(); i++) {
        auto oldItem = oldArtworksList.at(i)->getArtworkMetadata();
        auto newItem = newArtworksList.at(i)->getArtworkMetadata();

        VERIFY(oldItem->getItemID() != newItem->getItemID(), "Comparing same IDs");
        VERIFY(oldItem->getFilepath() == newItem->getFilepath(), "Filepaths don't match");
        VERIFY(oldItem->getTitle() == newItem->getTitle(), "Titles don't match");
        VERIFY(oldItem->getDescription() == newItem->getDescription(), "Descriptions don't match");
        VERIFY(oldItem->getKeywords() == newItem->getKeywords(), "Keywords don't match");

        Models::ImageArtwork *oldImage = dynamic_cast<Models::ImageArtwork*>(oldItem);
        Models::ImageArtwork *newImage = dynamic_cast<Models::ImageArtwork*>(newItem);
        if (oldImage != nullptr && newImage != nullptr) {
            VERIFY(oldImage->hasVectorAttached() == newImage->hasVectorAttached(), "Vector attachment lost");
            VERIFY(oldImage->getAttachedVectorPath() == newImage->getAttachedVectorPath(), "Vector filepath lost");
            VERIFY(oldImage->getImageSize().width() == newImage->getImageSize().width(), "Image widths don't match");
            VERIFY(oldImage->getImageSize().height() == newImage->getImageSize().height(), "Image heights don't match");
        }
    }

    // -------------------------------------
    // TODO: move below code away (maybe to perf tests)

    /*
    sessionManager->clearSession();

    QUrl path = getFilePathForTest("images-for-tests/pixmap/img_0007.jpg");
    QString pathString = path.toLocalFile();
    std::deque<Models::ArtworkMetadata *> metadataVector(10000, new Models::ArtworkMetadata(pathString, 0, 0));
    MetadataIO::SessionSnapshot sessionSnapshot(metadataVector);
    auto &snapshot = sessionSnapshot.getSnapshot();

    sessionManager->saveToFile(snapshot);

    sessionManager->clearSession();
    VERIFY(sessionManager->filesCount() == 0, "Session is not cleared");

    QTime timer;

    timer.start();
    {
        sessionManager->readSessionFromFile();
    }
    int elapsed = timer.elapsed();

    VERIFY(elapsed < 1000, "Session parsing is slow");
    VERIFY(sessionManager->filesCount() == 10000, "Session initialization failed");

    //qDeleteAll(metadataVector);
    */

    return 0;
}
