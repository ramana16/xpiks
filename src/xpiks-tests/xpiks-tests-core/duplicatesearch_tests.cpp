#include "duplicatesearch_tests.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "Mocks/spellcheckservicemock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManager; \
    Mocks::SpellCheckServiceMock spellCheckService; \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    commandManager.InjectDependency(&spellCheckService); \
    commandManager.InjectDependency(&artItemsModelMock); \
    Models::ArtworksRepository artworksRepository; \
    commandManager.InjectDependency(&artworksRepository); \
    Models::FilteredArtItemsProxyModel filteredItemsModel; \
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManager.InjectDependency(&filteredItemsModel);\
    Models::SettingsModel settingsModel; \
    settingsModel.initializeConfigs(); \
    settingsModel.setUseSpellCheck(true); \
    commandManager.InjectDependency(&settingsModel); \
    commandManager.generateAndAddArtworks(count);


void DuplicateSearchTests::simpleTest() {
    const int itemsToGenerate = 1;

    DECLARE_MODELS_AND_GENERATE(itemsToGenerate)
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    metadata->appendKeywords(QStringList() << "keyword_0" << "cat" << "keyword_1" << "keyword_2" << "cats");
    metadata->setTitle("title");
    metadata->setDescription("description");
    auto * basicModel = metadata->getBasicKeywordsModel();
    auto & spellCheckResults =  basicModel->getSpellCheckResults();
    spellCheckResults[0].m_Stems << "keyword_0";
    spellCheckResults[1].m_Stems  << "cat";
    spellCheckResults[2].m_Stems << "keyword_1";
    spellCheckResults[3].m_Stems << "keyword_2";
    spellCheckResults[4].m_Stems << "cat";
    basicModel->reevaluateStemsData();
    QVERIFY(!metadata->hasDuplicates(0));
    QVERIFY(metadata->hasDuplicates(1));
    QVERIFY(!metadata->hasDuplicates(2));
    QVERIFY(!metadata->hasDuplicates(3));
    QVERIFY(metadata->hasDuplicates(4));
}

void DuplicateSearchTests::distanceTest() {
    const int itemsToGenerate = 1;
    // for this test SYNONYMS_DISTANCE should be <= 3
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate)
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    metadata->appendKeywords(QStringList() << "keyword_0" << "tag" << "keyword_1" << "keyword_2" << "tagging");
    metadata->setTitle("title");
    metadata->setDescription("description");
    auto * basicModel = metadata->getBasicKeywordsModel();
    auto & spellCheckResults =  basicModel->getSpellCheckResults();
    spellCheckResults[0].m_Stems << "keyword_0";
    spellCheckResults[1].m_Stems  << "tag";
    spellCheckResults[2].m_Stems << "keyword_1";
    spellCheckResults[3].m_Stems << "keyword_2";
    spellCheckResults[4].m_Stems << "tag";
    basicModel->reevaluateStemsData();
    QVERIFY(!metadata->hasDuplicates(0));
    QVERIFY(!metadata->hasDuplicates(1));
    QVERIFY(!metadata->hasDuplicates(2));
    QVERIFY(!metadata->hasDuplicates(3));
    QVERIFY(!metadata->hasDuplicates(4));
}

void DuplicateSearchTests::severalwordsinkeywordTest() {
    const int itemsToGenerate = 1;

    DECLARE_MODELS_AND_GENERATE(itemsToGenerate)
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    metadata->appendKeywords(QStringList() << "keyword_0" << "cat" << "keyword_1" << "keyword_2" << "pretty cats");
    metadata->setTitle("title");
    metadata->setDescription("description");
    auto * basicModel = metadata->getBasicKeywordsModel();
    auto & spellCheckResults =  basicModel->getSpellCheckResults();
    spellCheckResults[0].m_Stems << "keyword_0";
    spellCheckResults[1].m_Stems << "cat";
    spellCheckResults[2].m_Stems  << "keyword_1";
    spellCheckResults[3].m_Stems   << "keyword_2";
    spellCheckResults[4].m_Stems  << "pretty" << "cat";
    basicModel->reevaluateStemsData();
    QVERIFY(!metadata->hasDuplicates(0));
    QVERIFY(metadata->hasDuplicates(1));
    QVERIFY(!metadata->hasDuplicates(2));
    QVERIFY(!metadata->hasDuplicates(3));
    QVERIFY(metadata->hasDuplicates(4));
}
