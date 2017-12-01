#include "autocompletepresetstest.h"
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
#include "testshelpers.h"

#define FIRST_PRESET "shutterstock"
#define SECOND_PRESET "canstockphoto"

QString AutoCompletePresetsTest::testName() {
    return QLatin1String("AutoCompletePresetsTest");
}

void AutoCompletePresetsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setUseKeywordsAutoComplete(true);
    settingsModel->setUsePresetsAutoComplete(true);

    KeywordsPresets::PresetKeywordsModel *presetsModel = m_CommandManager->getPresetsModel();
    bool dummyAdded;
    KeywordsPresets::ID_t dummyID;
    presetsModel->addOrUpdatePreset(FIRST_PRESET, QStringList() << "test" << "another" << "keyword", dummyID, dummyAdded);
    presetsModel->addOrUpdatePreset(SECOND_PRESET, QStringList() << "yet" << "other" << "keywords", dummyID, dummyAdded);
}

void AutoCompletePresetsTest::teardown() {
    IntegrationTestBase::teardown();

    KeywordsPresets::PresetKeywordsModel *presetsModel = m_CommandManager->getPresetsModel();
    KeywordsPresets::ID_t id;
    if (presetsModel->tryFindPresetByFullName(FIRST_PRESET, true, id)) {
        presetsModel->removePresetByID(id);
    }

    if (presetsModel->tryFindPresetByFullName(SECOND_PRESET, true, id)) {
        presetsModel->removePresetByID(id);
    }
}

int AutoCompletePresetsTest::doTest() {
    AutoComplete::AutoCompleteService *acService = m_CommandManager->getAutoCompleteService();
    AutoComplete::KeywordsAutoCompleteModel *acModel = acService->getAutoCompleteModel();
    AutoComplete::KeywordsCompletionsModel &completionsModel = acModel->getInnerModel();

    VERIFY(acModel->getCount() == 0, "AC model was not empty");

    // --------------------------------------------------------------

    xpiks()->generateCompletions("pr:stock", nullptr);

    sleepWaitUntil(5, [&]() {
        return completionsModel.getLastGeneratedCompletionsCount() > 0;
    });

    acModel->initializeCompletions();

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << completionsModel.getLastGeneratedCompletions();

    VERIFY(acModel->getCount() == 2, "Autocomplete did not find all presets");
    VERIFY(completionsModel.containsWord(FIRST_PRESET), "AC model did not find first preset");
    VERIFY(completionsModel.containsWord(SECOND_PRESET), "AC model did not find second preset");

    acModel->clear();

    // --------------------------------------------------------------

    xpiks()->generateCompletions("pr:shttrst", nullptr);

    sleepWaitUntil(5, [&]() {
        return completionsModel.getLastGeneratedCompletionsCount() > 0;
    });

    acModel->initializeCompletions();

    qInfo() << "Generated" << acModel->getCount() << "completions";
    qInfo() << "Completions:" << completionsModel.getLastGeneratedCompletions();

    VERIFY(acModel->getCount() == 1, "Autocomplete did not find all presets");
    VERIFY(completionsModel.containsWord(FIRST_PRESET), "Presets fuzzy search does not work");

    return 0;
}

