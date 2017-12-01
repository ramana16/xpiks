#include "undorestoresessiontest.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/sessionmanager.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/MetadataIO/artworkssnapshot.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"
#include "signalwaiter.h"
#include "testshelpers.h"

QString UndoRestoreSessionTest::testName() {
    return QLatin1String("UndoRestoreSessionTest");
}

void UndoRestoreSessionTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();

    settingsModel->setUseSpellCheck(false);
    settingsModel->setSaveSession(true);
    settingsModel->setAutoFindVectors(true);
}

int UndoRestoreSessionTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    Models::SessionManager *sessionManager = m_CommandManager->getSessionManager();
    VERIFY(sessionManager->itemsCount() == 0, "Session is not cleared");
    Models::ArtworksRepository *artworksRepository = m_CommandManager->getArtworksRepository();

    QList<QUrl> sources;
    sources << getFilePathForTest("images-for-tests/pixmap/img_0007.jpg")
            << getFilePathForTest("images-for-tests/pixmap/seagull-for-clear.jpg")
            << getFilePathForTest("images-for-tests/pixmap/seagull.jpg")
            << getDirPathForTest("images-for-tests/mixed/")
            << getDirPathForTest("images-for-tests/vector/");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    int addedCount = artItemsModel->dropFiles(sources);
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }
    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    sleepWaitUntil(10, [&]() {
        return sessionManager->itemsCount() == addedCount;
    });
    VERIFY(sessionManager->itemsCount() == addedCount, "Session does not contain all files");

    artworksRepository->resetEverything();
    artItemsModel->fakeDeleteAllItems();
    LOG_DEBUG << "About to restore...";

    int restoredCount = xpiks()->restoreSessionForTest();
    VERIFY(addedCount == restoredCount, "Failed to properly restore");
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading session metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();

    bool undoSuccess = undoRedoManager->undoLastAction();
    VERIFY(undoSuccess, "Failed to Undo last action");

    VERIFY(artItemsModel->getArtworksCount() == 0, "Items were not removed");

    return 0;
}
