#include "spellingproduceswarningstest.h"
#include <QUrl>
#include <QFileInfo>
#include <QThread>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/SpellCheck/spellsuggestionsitem.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"
#include "../../xpiks-qt/Warnings/warningsservice.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "testshelpers.h"

QString SpellingProducesWarningsTest::testName() {
    return QLatin1String("SpellingProducesWarningsTest");
}

void SpellingProducesWarningsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseSpellCheck(true);
}

int SpellingProducesWarningsTest::doTest() {
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

    Models::ArtworkMetadata *artwork = artItemsModel->getArtwork(0);
    VERIFY(artwork->isInitialized(), "Artwork is not initialized after import");

    sleepWait(3, [artwork]() {
        return !Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle) &&
                !Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription) &&
                !Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords);
    });

    VERIFY(!Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle), "Error for reading title");
    VERIFY(!Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription), "Error for reading description");
    VERIFY(!Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords), "Error for reading keywords");

    QString wrongWord = "abbreviatioe";
    artwork->setDescription(artwork->getDescription() + ' ' + wrongWord);
    artwork->setTitle(artwork->getTitle() + ' ' + wrongWord);
    artwork->appendKeyword("correct part " + wrongWord);
    artwork->setIsSelected(true);

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();

    SignalWaiter spellingWaiter;
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &spellingWaiter, SIGNAL(finished()));

    filteredModel->spellCheckSelected();

    if (!spellingWaiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    LOG_INFO << "Spellchecking finished. Waiting for warnings...";

    sleepWait(5, [=]() {
        return Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle) &&
                Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription) &&
                Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords);
    });

    auto *keywordsModel = artwork->getBasicModel();

    VERIFY(keywordsModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(keywordsModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(keywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    VERIFY(Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInTitle),
           "Warning was not produced for title spelling error");
    VERIFY(Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInDescription),
           "Warning was not produced for description spelling error");
    VERIFY(Common::HasFlag(artwork->getWarningsFlags(), Common::WarningFlags::SpellErrorsInKeywords),
           "Warning was not produced for keywords spelling error");

    return 0;
}
