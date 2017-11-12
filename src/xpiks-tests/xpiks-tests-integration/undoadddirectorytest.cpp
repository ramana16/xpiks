#include "undoadddirectorytest.h"
#include <QUrl>
#include <QFileInfo>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"

QString UndoAddDirectoryTest::testName() {
    return QLatin1String("UndoAddDirectoryTest");
}

void UndoAddDirectoryTest::setup() {
}

int UndoAddDirectoryTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();

    const auto dir = getDirPathForTest("images-for-tests/mixed/");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));
    QList<QUrl> dirs;
    dirs << dir;

    const int artworksCount = artItemsModel->addLocalDirectories(dirs);
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    // remove 2 artworks
    artItemsModel->removeItemsAtIndices({{0, 1}});
    VERIFY(artItemsModel->getArtworksCount() == artworksCount - 2, "Artworks were not removed");

    // remove directory
    artItemsModel->removeArtworksDirectory(0);

    VERIFY(artItemsModel->getArtworksCount() == 0, "All items were not removed");

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();

    bool undoSuccess = undoRedoManager->undoLastAction();
    VERIFY(undoSuccess, "Failed to Undo last action");

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(artItemsModel->getArtworksCount() == artworksCount, "Items were not put back");

    return 0;
}
