#include "duplicatesearchtest.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "signalwaiter.h"
#include "testshelpers.h"
#include <QObject>

QString DuplicateSearchTest::testName() {
    return QLatin1String("DuplicateSearchTest");
}

void DuplicateSearchTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();

    settingsModel->setUseSpellCheck(true);
}

int DuplicateSearchTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();

    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/mixed/cat.jpg");

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

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    metadata->clearModel();
    artItemsModel->pasteKeywords(0, QStringList() << "cat" << "Mouse" << "mice" << "on");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));

    filteredModel->spellCheckAllItems();

    QThread::sleep(1);

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    VERIFY(!metadata->hasDuplicates(0), "Duplicates detected for unique keyword");
    VERIFY(metadata->hasDuplicates(1), "Duplicate not detected for mouse");
    VERIFY(metadata->hasDuplicates(2), "Duplicate not detected for mice");

    artItemsModel->appendKeyword(0, "cats");
    artItemsModel->appendKeyword(0, "on");

    sleepWaitUntil(5, [&metadata]() {
        return metadata->hasDuplicates(0);
    });

    VERIFY(metadata->hasDuplicates(0), "Duplicates not detected singular");
    VERIFY(metadata->hasDuplicates(1), "Duplicate not detected for mouse after append");
    VERIFY(metadata->hasDuplicates(2), "Duplicate not detected for mice after append");
    VERIFY(metadata->hasDuplicates(3), "Duplicates not detected for plural");
    VERIFY(!metadata->hasDuplicates(4), "Duplicates detected for 2 chars word");

    return 0;
}
