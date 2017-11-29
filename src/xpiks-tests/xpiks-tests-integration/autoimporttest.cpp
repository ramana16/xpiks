#include "autoimporttest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/UndoRedo/undoredomanager.h"

QString AutoImportTest::testName() {
    return QLatin1String("AutoImportTest");
}

void AutoImportTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(false);
    settingsModel->setUseAutoImport(true);
}

int AutoImportTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/read-only/026.jpg");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    int addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == files.length(), "Failed to add file");

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for initial metadata reading.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");
    VERIFY(ioCoordinator->getImportIDs().size() == 1, "Import does not have any trace");

    QStringList expectedKeywords = QString("abstract,art,black,blue,creative,dark,decor,decoration,decorative,design,dot,drops,elegance,element,geometric,interior,light,modern,old,ornate,paper,pattern,purple,retro,seamless,style,textile,texture,vector,wall,wallpaper").split(',');

    VERIFY(expectedKeywords == artItemsModel->getArtwork(0)->getKeywords(), "Keywords are not the same after first import!");

    artItemsModel->removeArtworks({{0, 0}});

    UndoRedo::UndoRedoManager *undoRedoManager = m_CommandManager->getUndoRedoManager();

    bool undoSuccess = undoRedoManager->undoLastAction();
    VERIFY(undoSuccess, "Failed to Undo last action");

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata after undo.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");
    VERIFY(ioCoordinator->getImportIDs().size() == 2, "Undo import does not have any trace");

    VERIFY(expectedKeywords == artItemsModel->getArtwork(0)->getKeywords(), "Keywords are not the same after undo import!");

    return 0;
}

