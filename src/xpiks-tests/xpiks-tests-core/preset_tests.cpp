#include "preset_tests.h"
#include <thread>
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h"
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/Helpers/threadhelpers.h"
#include "../../xpiks-qt/Models/artworkproxymodel.h"

#define DECLARE_MODELS_AND_GENERATE(count) \
    Mocks::CommandManagerMock commandManagerMock; \
    Mocks::ArtItemsModelMock artItemsModelMock; \
    Models::ArtworksRepository artworksRepository; \
    Models::FilteredArtItemsProxyModel filteredItemsModel; \
    commandManagerMock.InjectDependency(&artworksRepository); \
    commandManagerMock.InjectDependency(&artItemsModelMock); \
    filteredItemsModel.setSourceModel(&artItemsModelMock); \
    commandManagerMock.InjectDependency(&filteredItemsModel); \
    commandManagerMock.generateAndAddArtworks(count); \
    KeywordsPresets::PresetKeywordsModel presetKeywordsModel; \
    commandManagerMock.InjectDependency(&presetKeywordsModel); \
    KeywordsPresets::FilteredPresetKeywordsModel filteredPresetKeywordsModel; \
    filteredPresetKeywordsModel.setSourceModel(&presetKeywordsModel);

void PresetTests::expandFromPresetTrivial()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList() << "keyword_0");
    }

    artItemsModelMock.expandPreset(0, 0, 0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::expandFromPresetWithDuplicates()
{
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    artItemsModelMock.expandPreset(0, 0, 0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::appendFromPresetTrivial() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    artItemsModelMock.addPreset(0, 0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_0" << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::appendFromPresetWithDuplicates() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    presetKeywordsModel.addItem();
    presetKeywordsModel.setName(0, QString("keyword_0"));
    presetKeywordsModel.appendKeyword(0, "keyword_1");
    presetKeywordsModel.appendKeyword(0, "keyword_2");
    presetKeywordsModel.appendKeyword(0, "keyword_3");
    presetKeywordsModel.appendKeyword(0, "keyword_4");
    presetKeywordsModel.appendKeyword(0, "keyword_5");

    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList() << "keyword_0" << "keyword_1" << "keyword_2");
    }

    artItemsModelMock.addPreset(0, 0);
    Models::ArtworkMetadata *metadata = artItemsModelMock.getArtwork(0);
    QStringList finalString;
    finalString << "keyword_0" << "keyword_1" << "keyword_2" << "keyword_3" << "keyword_4" << "keyword_5";
    QCOMPARE(metadata->getKeywords(), finalString);
    QVERIFY(metadata->isModified());
}

void PresetTests::appendToProxyModelTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList());
    }

    Models::ArtworkProxyModel proxyModel;
    commandManagerMock.InjectDependency(&proxyModel);
    proxyModel.setSourceArtwork((QObject*)artItemsModelMock.getMockArtwork(0));
    artItemsModelMock.setUpdatesBlocked(false);

    QStringList keywords;
    keywords << "some" << "keywords";

    KeywordsPresets::ID_t id1 = presetKeywordsModel.addItem("test", keywords);
    proxyModel.addPreset(id1);

    QCOMPARE(artItemsModelMock.getMockArtwork(0)->getKeywords(), keywords);
    QVERIFY(artItemsModelMock.getMockArtwork(0)->isModified());
}

void PresetTests::expandLastKeywordInProxyModelTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    for (int i = 0; i < itemsToGenerate; i++) {
        auto *metadata = artItemsModelMock.getMockArtwork(i);
        metadata->set("title", "description", QStringList() << "keyword_0" << "preset name");
    }

    Models::ArtworkProxyModel proxyModel;
    commandManagerMock.InjectDependency(&proxyModel);
    proxyModel.setSourceArtwork((QObject*)artItemsModelMock.getMockArtwork(0));
    artItemsModelMock.setUpdatesBlocked(false);

    QStringList keywords;
    keywords << "some" << "keywords";

    KeywordsPresets::ID_t id1 = presetKeywordsModel.addItem("preset name", keywords);
    Q_UNUSED(id1);
    proxyModel.expandLastKeywordAsPreset();

    QVERIFY(artItemsModelMock.getMockArtwork(0)->hasKeywords(keywords));
    QVERIFY(artItemsModelMock.getMockArtwork(0)->isModified());
}

void PresetTests::findPresetByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    KeywordsPresets::ID_t id1 = presetKeywordsModel.addItem("man", QStringList() << "some" << "keywords");
    KeywordsPresets::ID_t id2 = presetKeywordsModel.addItem("woman", QStringList() << "other" << "keywords");

    KeywordsPresets::ID_t id;

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("man", false, id)); QCOMPARE(id, id1);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("woman", false, id)); QCOMPARE(id, id2);

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("mAn", false, id)); QCOMPARE(id, id1);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("WomaN", false, id)); QCOMPARE(id, id2);

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("an", false, id));
}

void PresetTests::strictFindPresetByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    KeywordsPresets::ID_t id1 = presetKeywordsModel.addItem("Man", QStringList() << "some" << "keywords");
    KeywordsPresets::ID_t id2 = presetKeywordsModel.addItem("Woman", QStringList() << "other" << "keywords");

    KeywordsPresets::ID_t id;

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("man", true, id));
    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", true, id));

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Man", true, id)); QCOMPARE(id, id1);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Woman", true, id)); QCOMPARE(id, id2);
}

void PresetTests::findPresetWithLongNamesByNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);
    KeywordsPresets::ID_t id1 = presetKeywordsModel.addItem("young woman", QStringList() << "some" << "keywords");
    KeywordsPresets::ID_t id2 = presetKeywordsModel.addItem("old woman", QStringList() << "other" << "keywords");

    KeywordsPresets::ID_t id;

    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", false, id));
    QVERIFY(!presetKeywordsModel.tryFindSinglePresetByName("woman", true, id));

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("young", false, id)); QCOMPARE(id, id1);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("old", false, id)); QCOMPARE(id, id2);

    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("Young woman", false, id)); QCOMPARE(id, id1);
    QVERIFY(presetKeywordsModel.tryFindSinglePresetByName("old Woman", false, id)); QCOMPARE(id, id2);
}

void PresetTests::addPresetKeywordsWithDuplicatesTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    bool isAdded = false;
    KeywordsPresets::ID_t id = 0;
    presetKeywordsModel.addOrUpdatePreset("bike", QStringList() << "downhill" << "slope" <<
                                          "uphill" << "slope", id, isAdded);

    QVERIFY(isAdded == true);
    QStringList keywords;
    bool found = presetKeywordsModel.tryGetPreset(id, keywords);
    QVERIFY(found);

    QStringList expectedKeywords;
    expectedKeywords << "downhill" << "slope" << "uphill";

    QCOMPARE(keywords, expectedKeywords);
}

void PresetTests::addRemovePresetTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    bool isAdded = false;

    int n = 10;
    while (n--) {
        qDebug() << (5 - n - 1);
        KeywordsPresets::ID_t id = 0;
        presetKeywordsModel.addOrUpdatePreset("bike", QStringList() << "downhill" << "slope" <<
                                              "uphill" << "slope", id, isAdded);
        QCOMPARE(isAdded, true);

        bool isRemoved = presetKeywordsModel.removePresetByID(id);
        QCOMPARE(isRemoved, true);
    }
}

void PresetTests::parallelAccessTest() {
    const int initIterations = 100;
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    QString presetName = "preset";
    QStringList keywords;
    keywords << "keyword1" << "another keyword" << "test";

    Helpers::Barrier barrier1(2), barrier2(2);
    bool isAdded1, isAdded2;
    volatile bool removed1;
    volatile bool removed2;
    KeywordsPresets::ID_t id1 = 0;
    KeywordsPresets::ID_t id2 = 0;

    std::thread thread1([&]() {
        int n = initIterations;
        while (n--) {
            isAdded1 = false; id1 = 0;
            presetKeywordsModel.addOrUpdatePreset(presetName, keywords, id1, isAdded1);

            /*-------------*/barrier1.wait();

            Q_ASSERT(isAdded1 != isAdded2);
            Q_ASSERT(id1 == id2);

            removed1 = presetKeywordsModel.removePresetByID(id1);

            /*-------------*/barrier2.wait();

            Q_ASSERT(removed1 != removed2);
        }
    });

    std::thread thread2([&]() {

        int n = initIterations;
        while (n--) {
            isAdded2 = false; id2 = 1;
            presetKeywordsModel.addOrUpdatePreset(presetName, keywords, id2, isAdded2);

            /*-------------*/barrier1.wait();

            Q_ASSERT(isAdded1 != isAdded2);
            Q_ASSERT(id1 == id2);

            removed2 = presetKeywordsModel.removePresetByID(id2);

            /*-------------*/barrier2.wait();

            Q_ASSERT(removed1 != removed2);
        }
    });

    thread1.join();
    thread2.join();
}

void PresetTests::addAndRetrieveTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id = presetKeywordsModel.addItem(presetName, keywords);

    QStringList actual;
    presetKeywordsModel.tryGetPreset(id, actual);

    QCOMPARE(actual, keywords);
}

void PresetTests::registerTwiceTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id1;
    presetKeywordsModel.findOrRegisterPreset(presetName, keywords, id1);

    KeywordsPresets::ID_t id2;
    presetKeywordsModel.findOrRegisterPreset(presetName, keywords, id2);

    QCOMPARE(id2, id1);
}

void PresetTests::addAndRegisterTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id1;
    bool isAdded = false;
    presetKeywordsModel.addOrUpdatePreset(presetName, keywords, id1, isAdded);
    QCOMPARE(isAdded, true);

    KeywordsPresets::ID_t id2;
    presetKeywordsModel.findOrRegisterPreset(presetName, keywords, id2);

    QCOMPARE(id2, id1);
}

void PresetTests::addPresetTwiceTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id1;
    bool isAdded = false;
    presetKeywordsModel.addOrUpdatePreset(presetName, keywords, id1, isAdded);
    QCOMPARE(isAdded, true);

    KeywordsPresets::ID_t id2;
    bool isAdded2 = false;
    const QStringList keywords2 = QStringList() << "keyword5" << "something" << "here";
    presetKeywordsModel.addOrUpdatePreset(presetName, keywords2, id2, isAdded2);
    QCOMPARE(isAdded2, false);
    QCOMPARE(id2, id1);

    QStringList actualKeyowrds;
    presetKeywordsModel.tryGetPreset(id2, actualKeyowrds);
    QCOMPARE(actualKeyowrds, keywords2);
}

void PresetTests::registerWithAnotherNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id1;
    presetKeywordsModel.findOrRegisterPreset(presetName, keywords, id1);

    KeywordsPresets::ID_t id2;
    presetKeywordsModel.findOrRegisterPreset(presetName + "1", keywords, id2);

    QVERIFY(id2 != id1);
}

void PresetTests::addWithAnotherNameTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString presetName = "preset";
    const QStringList keywords = QStringList() << "keyword1" << "test" << "another";

    KeywordsPresets::ID_t id1;
    bool isAdded = false;
    presetKeywordsModel.addOrUpdatePreset(presetName, keywords, id1, isAdded);
    QCOMPARE(isAdded, true);

    KeywordsPresets::ID_t id2;
    bool isAdded2 = false;
    const QStringList keywords2 = QStringList() << "keyword5" << "something" << "here";
    presetKeywordsModel.addOrUpdatePreset(presetName + "1", keywords2, id2, isAdded2);
    QCOMPARE(isAdded2, true);
    QVERIFY(id2 != id1);

    QStringList actualKeyowrds;
    presetKeywordsModel.tryGetPreset(id2, actualKeyowrds);
    QCOMPARE(actualKeyowrds, keywords2);
}

void PresetTests::registerGroupTwiceTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    const QString groupName = "group1";

    int groupID1 = 0, groupID2 = 1;

    presetKeywordsModel.findOrRegisterGroup(groupName, groupID1);
    presetKeywordsModel.findOrRegisterGroup(groupName, groupID2);

    QCOMPARE(groupID2, groupID1);
}

void PresetTests::setPresetUnknownGroupTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    bool isAdded = false;
    KeywordsPresets::ID_t id = 0;
    presetKeywordsModel.addOrUpdatePreset("bike", QStringList() << "downhill" << "slope" <<
                                          "uphill" << "slope", id, isAdded);

    bool setGroupSuccess = presetKeywordsModel.setPresetGroup(id, 1234);
    QCOMPARE(setGroupSuccess, false);

    int groupID = 0;
    presetKeywordsModel.findOrRegisterGroup("test group", groupID);

    setGroupSuccess = presetKeywordsModel.setPresetGroup(id, groupID);
    QCOMPARE(setGroupSuccess, true);
}

void PresetTests::addGroupTest() {
    const int itemsToGenerate = 5;
    DECLARE_MODELS_AND_GENERATE(itemsToGenerate);

    bool isAdded = false;
    KeywordsPresets::ID_t id = 0;
    presetKeywordsModel.addOrUpdatePreset("bike", QStringList() << "downhill" << "slope" <<
                                          "uphill" << "slope", id, isAdded);

    int groupID = 0;
    presetKeywordsModel.findOrRegisterGroup("test group", groupID);

    bool success = presetKeywordsModel.setPresetGroup(id, groupID);
    QVERIFY(success);

    int foundID = 0;
    bool foundGroup = presetKeywordsModel.tryGetGroupFromIndex(0, foundID);
    QVERIFY(foundGroup);
    QCOMPARE(foundID, groupID);
}
