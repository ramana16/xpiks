#include "addspellingwordtests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/SpellCheck/spellcheckiteminfo.h"
#include "Mocks/spellcheckservicemock.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include <QObject>

#define INIT_ADD_SPELLING_TEST(count) \
    Mocks::CommandManagerMock commandManager; \
    Mocks::SpellCheckServiceMock spellCheckService; \
    commandManager.InjectDependency(&spellCheckService); \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    Models::ArtworksRepository artworksRepository; \
    commandManager.InjectDependency(&artworksRepository); \
    commandManager.InjectDependency(&artItemsModelMock); \
    commandManager.generateAndAddArtworks(count); \
    QObject::connect(&spellCheckService, SIGNAL(addedUserWordToDictionary(QString)), &artItemsModelMock, SIGNAL(addedUserWordToDictionary(QString)));

void AddSpellingWordTests::addWordSignalTest() {
    INIT_ADD_SPELLING_TEST(2)
    Models::ArtworkMetadata *metadata0 = artItemsModelMock.getArtwork(0);
    Models::ArtworkMetadata *metadata1 = artItemsModelMock.getArtwork(1);
    QSignalSpy addSpellWordSpy1(metadata0->getKeywordsModel(), SIGNAL(spellCheckErrorsChanged()));
    QSignalSpy addSpellWordSpy2(metadata1->getKeywordsModel(), SIGNAL(spellCheckErrorsChanged()));
    QSignalSpy addSpellWordSpy3(metadata0->getKeywordsModel(), SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));
    QSignalSpy addSpellWordSpy4(metadata1->getKeywordsModel(), SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)));

    metadata0->initialize("frtg", "keke abcdfe", QStringList() <<  "kdfdfdg" << "keke" << "abcdfe" << "frtg");
    metadata1->initialize("frtg", "keke abcdfe", QStringList() <<  "kdfdfdg" << "keke" << "abcdfe" << "frtg");
    spellCheckService.addedUserWordToDictionary("keke");

    QCOMPARE(addSpellWordSpy1.count(), 1);
    QCOMPARE(addSpellWordSpy2.count(), 1);
    QCOMPARE(addSpellWordSpy3.count(), 1);
    QCOMPARE(addSpellWordSpy4.count(), 1);
}
