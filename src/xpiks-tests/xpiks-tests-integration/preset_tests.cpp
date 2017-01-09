#include "preset_tests.h"
#include "../xpiks-qt/PresetKeywords/presetkeywordsmodel.h"
#include "../xpiks-qt/PresetKeywords/presetkeywordsmodelconfig.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include <QDir>

QString PresetTests::testName() {
    return QLatin1String("preset_tests");
}

void PresetTests::setup() {
}

int PresetTests::doTest() {

    KeywordsPreset::PresetKeywordsModel *presetKeywordsModel = m_CommandManager->getPresetsModel();
    KeywordsPreset::PresetKeywordsModelConfig *presetKeywordsModelConfig = m_CommandManager->getPresetsModelConfig();

    typedef KeywordsPreset::PresetData PresetData;
    QVector<PresetData> presetDataVector;
    presetDataVector.push_back({QStringList() << "key1" << "key2", "name1"});
    presetDataVector.push_back({QStringList() << "key3" << "key4", "name2"});
    presetKeywordsModelConfig->initialize(presetDataVector);

    presetKeywordsModel->removeKeywordAt(0,0);
    presetKeywordsModel->appendKeyword(1,"key5");
    presetKeywordsModel->addItem();
    presetKeywordsModel->setName(2,QString("name3"));
    presetKeywordsModel->appendKeyword(2,"key6");
    presetKeywordsModel->saveToConfig();

    QVector<PresetData> goldPresetDataVector;
    goldPresetDataVector.push_back({QStringList() << "key2", "name1"});
    goldPresetDataVector.push_back({QStringList() << "key3" << "key4" << "key5", "name2"});
    goldPresetDataVector.push_back({QStringList() << "key6", "name3"});

    QThread::sleep(1);

    auto &presetDataNew = presetKeywordsModelConfig->getPresetData();
    int size = presetDataNew.size();
    VERIFY(size == goldPresetDataVector.size(), "Error in verifying config data size");

    for (int i = 0; i < size; i++) {
        VERIFY(presetDataNew[i].m_Keys == goldPresetDataVector[i].m_Keys, "Error in verifying config data item keywords");
        VERIFY(presetDataNew[i].m_Name == goldPresetDataVector[i].m_Name, "Error in verifying config data item name");
    }
    return 0;
}
