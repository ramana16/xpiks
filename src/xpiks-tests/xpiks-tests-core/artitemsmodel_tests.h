#ifndef ARTITEMSMODELTESTS_H
#define ARTITEMSMODELTESTS_H

#include <QObject>
#include <QtTest/QTest>

class ArtItemsModelTests : public QObject
{
    Q_OBJECT
private slots:
    void removeUnavailableTest();
    void unselectAllTest();
    void modificationChangesModifiedCountTest();
    void removeArtworkDirectorySimpleTest();
    void addRemoveOneByOneFewDirsTest();
    void addRemoveOneByOneOneDirTest();
    void setAllSavedResetsModifiedCountTest();
    void removingLockedArtworksTest();
    void plainTextEditToEmptyKeywordsTest();
    void plainTextEditToOneKeywordTest();
    void plainTextEditToSeveralKeywordsTest();
    void plainTextEditToAlmostEmptyTest();
    void plainTextEditToMixedTest();
    // smoke tests for modified state
    void appendKeywordEmitsModifiedTest();
    void removeKeywordEmitsModifiedTest();
    void removeLastKeywordEmitsModifiedTest();
    void plainTextEditEmitsModifiedTest();
    void keywordEditEmitsModifiedTest();
    void pasteKeywordsEmitsModifiedTest();
    void addSuggestedEmitsModifiedTest();
    void fillFromQuickBufferEmitsModifiedTest();
    void addPresetEmitsModifiedTest();
    void proxyModelExitEmitsModifiedTest();
};

#endif // ARTITEMSMODELTESTS_H
