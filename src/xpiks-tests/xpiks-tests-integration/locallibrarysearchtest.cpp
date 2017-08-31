#include "locallibrarysearchtest.h"
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
#include "../../xpiks-qt/Suggestion/keywordssuggestor.h"
#include "testshelpers.h"

QString LocalLibrarySearchTest::testName() {
    return QLatin1String("LocalLibrarySearchTest");
}

void LocalLibrarySearchTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(false);
}

int LocalLibrarySearchTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/vector/026.jpg");

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

    artItemsModel->initSuggestion(0);

    Suggestion::KeywordsSuggestor *suggestor = m_CommandManager->getKeywordsSuggestor();

    suggestor->setSelectedSourceIndex(suggestor->getEngineNames().size() - 1); // local search is the last one
    VERIFY(suggestor->getIsLocalSearch(), "Local search cannot be chosen");

    // 0 was meant to be "any search result"
    suggestor->searchArtworks("abstract", 0);
    VERIFY(suggestor->getIsInProgress(), "Keywords suggestor did not start");

    sleepWaitUntil(5, [suggestor]() {
        return suggestor->getIsInProgress() == false;
    });

    VERIFY(suggestor->getIsInProgress() == false, "Keywords suggestor is still working");

    VERIFY(suggestor->rowCount() >= 2, "Artworks cannot be found");

    return 0;
}
