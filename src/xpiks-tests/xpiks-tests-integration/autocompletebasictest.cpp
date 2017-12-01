#include "autocompletebasictest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/AutoComplete/autocompleteservice.h"
#include "../../xpiks-qt/AutoComplete/keywordsautocompletemodel.h"

QString AutoCompleteBasicTest::testName() {
    return QLatin1String("AutoCompleteBasicTest");
}

void AutoCompleteBasicTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseKeywordsAutoComplete(true);
}

int AutoCompleteBasicTest::doTest() {
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

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    Common::BasicMetadataModel *basicModel = metadata->getBasicModel();

    AutoComplete::AutoCompleteService *acService = m_CommandManager->getAutoCompleteService();
    AutoComplete::KeywordsAutoCompleteModel *acModel = acService->getAutoCompleteModel();
    AutoComplete::KeywordsCompletionsModel &completionsModel = acModel->getInnerModel();

    SignalWaiter completionWaiter;
    QObject::connect(basicModel, SIGNAL(completionsAvailable()), &completionWaiter, SIGNAL(finished()));

    VERIFY(acModel->getCount() == 0, "AC model was not empty");

    // --------------------------------------------------------------

    xpiks()->generateCompletions("tes", metadata->getBasicModel());

    if (!completionWaiter.wait(10)) {
        VERIFY(false, "Timeout while waiting for the completion");
    }

    acModel->initializeCompletions();

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << completionsModel.getLastGeneratedCompletions();

    VERIFY(acModel->getCount() > 0, "AC model didn't receive the completions");
    VERIFY(completionsModel.containsWord("test"), "AC model has irrelevant results");

    acModel->clear();

    // --------------------------------------------------------------

    xpiks()->generateCompletions("Tes", metadata->getBasicModel());

    if (!completionWaiter.wait(10)) {
        VERIFY(false, "Timeout while waiting for the completion");
    }

    acModel->initializeCompletions();

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << completionsModel.getLastGeneratedCompletions();

    VERIFY(acModel->getCount() > 0, "AC model didn't receive the completions second time");
    VERIFY(completionsModel.containsWord("test"), "AC model has irrelevant results");

    // --------------------------------------------------------------

    VERIFY(acModel->moveSelectionDown(), "AC model can't move selection down");
    // in the beginning the selection index is -1
    VERIFY(acModel->moveSelectionDown(), "AC model can't move selection down");
    VERIFY(acModel->moveSelectionUp(), "AC model can't move selection back up");
    VERIFY(!acModel->moveSelectionUp(), "AC model can move selection back up while being at a top");

    return 0;
}
