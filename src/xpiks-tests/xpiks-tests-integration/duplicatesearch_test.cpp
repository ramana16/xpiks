#include "duplicatesearch_test.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/SpellCheck/spellcheckerservice.h"
#include "signalwaiter.h"
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
    metadata->clearKeywords();
    artItemsModel->pasteKeywords(0, QStringList() << "cat" << "dog" << "dogs");

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    SpellCheck::SpellCheckerService *spellCheckService = m_CommandManager->getSpellCheckerService();
    QObject::connect(spellCheckService, SIGNAL(spellCheckQueueIsEmpty()), &waiter, SIGNAL(finished()));

    filteredModel->spellCheckAllItems();
    QThread::sleep(1);
    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    VERIFY(!metadata->hasDuplicates(0), "0 Error in synonyms status for 0");
    VERIFY(metadata->hasDuplicates(1), "0 Error in synonyms status for 1");
    VERIFY(metadata->hasDuplicates(2), "0 Error in synonyms status for 2");

    artItemsModel->appendKeyword(0, "cats");

    filteredModel->spellCheckAllItems();
    QThread::sleep(1);
    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    VERIFY(metadata->hasDuplicates(0), "1 Error in synonyms status for 0");
    VERIFY(metadata->hasDuplicates(1), "1 Error in synonyms status for 1");
    VERIFY(metadata->hasDuplicates(2), "1 Error in synonyms status for 2");
    VERIFY(metadata->hasDuplicates(3), "1 Error in synonyms status for 3");

    metadata->clearKeywords();
    QThread::sleep(3);
    artItemsModel->pasteKeywords(0, QStringList() << "cat" << "some");
    QThread::sleep(3);
    auto *presetKeywordsModel = m_CommandManager->getPresetsModel();
    presetKeywordsModel->addItem();
    int size = presetKeywordsModel->getPresetsCount();
    presetKeywordsModel->setName(0, QString("name"));
    presetKeywordsModel->appendKeyword(size - 1, "cats");
    presetKeywordsModel->appendKeyword(size - 1, "eye");
    presetKeywordsModel->appendKeyword(size - 1, "eyes");
    presetKeywordsModel->saveToConfig();
    artItemsModel->expandPreset(0, 1, size - 1);

    filteredModel->spellCheckAllItems();
    QThread::sleep(1);
    if (!waiter.wait(5)) {
        VERIFY(false, "Timeout for waiting for first spellcheck results");
    }

    VERIFY(metadata->hasDuplicates(0), "2 Error in synonyms status for 0");
    VERIFY(metadata->hasDuplicates(1), "2 Error in synonyms status for 1");
    VERIFY(metadata->hasDuplicates(2), "2 Error in synonyms status for 2");
    VERIFY(metadata->hasDuplicates(3), "2 Error in synonyms status for 3");
    metadata->clearKeywords();

    return 0;
}
