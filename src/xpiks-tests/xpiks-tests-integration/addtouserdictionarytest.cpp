#include "addtouserdictionarytest.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
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
    files << getImagePathForTest("images-for-tests/vector/026.jpg");
    files << getImagePathForTest("images-for-tests/vector/027.jpg");

    int addedCount = artItemsModel->addLocalArtworks(files);

    VERIFY(addedCount == files.length(), "Failed to add file");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));

    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);

    QString wrongWord = "abbreviatioe";
    metadata->setDescription(metadata->getDescription() + ' ' + wrongWord);
    metadata->setTitle(metadata->getTitle() + ' ' + wrongWord);
    metadata->appendKeyword("correct part " + wrongWord);
    metadata->setIsSelected(true);

    // wait for after-add spellchecking
    QThread::sleep(1);

    Common::BasicKeywordsModel *basicKeywordsModel = metadata->getKeywordsModel();

    VERIFY(basicKeywordsModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(basicKeywordsModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    SpellCheck::SpellCheckerService *spellcheckService = m_CommandManager->getSpellCheckerService();
    spellcheckService->addWordToUserDictionary(wrongWord);

    // wait add user word to finish
    QThread::sleep(1);

    int userDictWords = spellcheckService->getUserDictWordsNumber();
    VERIFY(userDictWords == 1, "Wrong number of words in user dictionary");

    VERIFY(!basicKeywordsModel->hasDescriptionSpellError(), "Description spell error is still present");
    VERIFY(!basicKeywordsModel->hasTitleSpellError(), "Title spell error is still present");
    VERIFY(!basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error is still present");

    spellcheckService->clearUserDictionary();
    // wait clear user dict to finish
    QThread::sleep(1);

    userDictWords = spellcheckService->getUserDictWordsNumber();
    VERIFY(userDictWords == 0, "User dictionary was not cleared");

    VERIFY(basicKeywordsModel->hasDescriptionSpellError(), "Description spell error not detected");
    VERIFY(basicKeywordsModel->hasTitleSpellError(), "Title spell error not detected");
    VERIFY(basicKeywordsModel->hasKeywordsSpellError(), "Keywords spell error not detected");

    return 0;
}
