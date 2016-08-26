#include "addtouserdictionarytest.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/combinedartworksmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "signalwaiter.h"
#include <QObject>

QString AddToUserDictionaryTest::testName() {
    return QLatin1String("AddToUserDictionaryTest");
}

void AddToUserDictionaryTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();

    settingsModel->setUseSpellCheck(true);
}

int AddToUserDictionaryTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();

    QList<QUrl> files;
    files << getImagePathForTest("images-for-tests/pixmap/seagull.jpg");

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add file")

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    filteredModel->selectFilteredArtworks();
    filteredModel->combineSelectedArtworks();

    // wait for after-add spellchecking
    QThread::sleep(1);

    Models::CombinedArtworksModel *combinedModel = m_CommandManager->getCombinedArtworksModel();
    Common::BasicKeywordsModel *basicModel = combinedModel->getBasicKeywordsModel();

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    QString wrongWord = "abbreviatioe";
    metadata->setDescription(metadata->getDescription() + ' ' + wrongWord);
    metadata->setTitle(metadata->getTitle() + ' ' + wrongWord);
    metadata->appendKeyword("correct part " + wrongWord);

    // wait for after-add spellchecking
    QThread::sleep(1);

//    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));

    filteredModel->spellCheckSelected();

    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for spellcheck results");
    }

    // wait for finding suggestions
    QThread::sleep(1);

    VERIFY(basicModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(basicModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(basicModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    SpellCheck::SpellCheckerService *spellcheckService = m_CommandManager->getSpellCheckerService();
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));
    spellcheckService->addWordToUserDictionary(wrongWord);

    // wait add user word to finish
    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for spellcheck results");
    }

    int userDictWords = spellcheckService->getUserDictWordsNumber();
    VERIFY(userDictWords == 1, "Wrong number of words in user dictionary");
    VERIFY(!basicModel->hasDescriptionSpellError(), "After adding word. Description spell error is still present");
    VERIFY(!basicModel->hasTitleSpellError(), "After adding word. Title spell error is still present");
    VERIFY(!basicModel->hasKeywordsSpellError(), "After adding word. Keywords spell error is still present");

    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));
    spellcheckService->clearUserDictionary();
    // wait clear user dict to finish
    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for spellcheck results");
    }

    userDictWords = spellcheckService->getUserDictWordsNumber();
    VERIFY(userDictWords == 0, "User dictionary was not cleared");
    VERIFY(basicModel->hasDescriptionSpellError(), "After clear. Description spell error not detected");
    VERIFY(basicModel->hasTitleSpellError(), "After clear. Title spell error not detected");
    VERIFY(basicModel->hasKeywordsSpellError(), "After clear. Keywords spell error not detected");

    return 0;
}
