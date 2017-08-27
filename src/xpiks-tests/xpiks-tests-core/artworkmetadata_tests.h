#ifndef ARTWORKMETADATA_TESTS_H
#define ARTWORKMETADATA_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class ArtworkMetadataTests: public QObject
{
    Q_OBJECT
private slots:
    void initializeBlancFromOriginTest();
    void initializeEmptyFromCachedTest();
    void initializeNotEmptyFromCachedSkipTest();
    void initializeOriginalWithEmpty();
    void initOriginThenStorageTest();
    void initStorageThenOriginTest();
    void initOriginalThenStorageDescriptionEmptyTest();
    void initOriginalThenStorageTitleEmptyTest();
    void initStorageThenOriginKeywordsOverwriteTest();
    void initStorageThenOriginKeywordsAddTest();
    void initStorageThenOriginShouldModifyTest();
    void parallelInitOriginStorageTest();
    void parallelInitEmptyStorageTest();
    void parallelInitStorageSetTest();
    void parallelInitOriginSetTest();
    void freshObjectTest();
    void markModifiedShouldEmitSignalTest();
    void modifiedIsNotMarkedModifiedAgainTest();
    void setDescriptionEmitsModifiedTest();
    void setTitleEmitsModifiedTest();
    void addNewKeywordsEmitsModifiedTest();
    void addExistingKeywordsDoesNotEmitModifiedTest();
    void addOneNewKeywordEmitsModifiedTest();
    void addOneExistingKeywordDoesNotEmitModifiedTest();
    void removeKeywordFromEmptyTest();
    void removeLastKeywordFromEmptyTest();
    void removeActualKeywordTest();
    void removeLastActualKeywordTest();
    void editKeywordToAnotherEmitsModifiedTest();
    void editKeywordToExistingDoesNotEmitModifiedTest();
    void misEditOfKeywordDoesNothingTest();
    void isInDirectoryTest();
    void isNotInParentsDirectoryTest();
    void isNotInOtherDirectoryTest();
    void isNotInEmptyDirectoryTest();
    void clearKeywordsMarksAsModifiedTest();
    void clearEmptyKeywordsDoesNotMarkModifiedTest();
    void removeKeywordsMarksModifiedTest();
};

#endif // ARTWORKMETADATA_TESTS_H
