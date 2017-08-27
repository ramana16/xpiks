#include "artworkmetadata_tests.h"
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <QSemaphore>
#include "Mocks/artworkmetadatamock.h"
#include "../../xpiks-qt/MetadataIO/cachedartwork.h"
#include "../../xpiks-qt/MetadataIO/originalmetadata.h"
#include <thread>
#include "../../xpiks-qt/Helpers/threadhelpers.h"

MetadataIO::CachedArtwork CA(const QString &title, const QString &description, const QStringList &keywords) {
    MetadataIO::CachedArtwork result;
    result.m_Title = title;
    result.m_Description = description;
    result.m_Keywords = keywords;
    return result;
}

MetadataIO::OriginalMetadata OM(const QString &title, const QString &description, const QStringList &keywords) {
    MetadataIO::OriginalMetadata result;
    result.m_Title = title;
    result.m_Description = description;
    result.m_Keywords = keywords;
    return result;
}

void ArtworkMetadataTests::initializeBlancFromOriginTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initFromOrigin(OM(title, description, rawKeywords));

    // overwrite in this case is true
    QCOMPARE(result, true);

    QCOMPARE(metadata.getTitle(), title);
    QCOMPARE(metadata.getDescription(), description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword2"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword3"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initializeEmptyFromCachedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

    QCOMPARE(metadata.isInitialized(), false);

    bool result = metadata.initFromStorage(CA(title, description, rawKeywords));

    // overwrite in this case is false, but metadata is empty
    QCOMPARE(result, true);

    QCOMPARE(metadata.getTitle(), title);
    QCOMPARE(metadata.getDescription(), description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword2"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword3"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.isAlmostInitialized(), true);
}

void ArtworkMetadataTests::initializeNotEmptyFromCachedSkipTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    bool result = metadata.initFromOrigin(OM("Existing Title", "Existing Description", QStringList() << "keyword1" << "keyword3" << "keyword2"));
    QCOMPARE(result, true);

    QString title = "Artwork Special Title";
    QString description = "Artwork Special Description";
    QStringList rawKeywords;
    rawKeywords << "keyword1" << "keyword2" << "keyword3";

    QCOMPARE(metadata.isInitialized(), true);

    result = metadata.initFromStorage(CA(title, description, rawKeywords));

    // no items will be overwritten
    QCOMPARE(result, false);

    QVERIFY(metadata.getTitle() != title);
    QVERIFY(metadata.getDescription() != description);
    QCOMPARE(metadata.rowCount(), 3);
    QCOMPARE(metadata.retrieveKeyword(0), QLatin1String("keyword1"));
    QCOMPARE(metadata.retrieveKeyword(1), QLatin1String("keyword3"));
    QCOMPARE(metadata.retrieveKeyword(2), QLatin1String("keyword2"));

    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initializeOriginalWithEmpty() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    metadata.initFromStorage(CA("Existing Title", "Existing Desc", QStringList() << "keyword1" << "keyword3" << "keyword2"));
    metadata.resetModified();

    QString title;
    QString description;
    QStringList rawKeywords;

    bool result = metadata.initFromOrigin(OM(title, description, rawKeywords));

    QCOMPARE(result, false);

    QVERIFY(!metadata.isTitleEmpty());
    QVERIFY(!metadata.isDescriptionEmpty());
    QVERIFY(!metadata.areKeywordsEmpty());

    QCOMPARE(metadata.isInitialized(), true);
}

void ArtworkMetadataTests::initOriginThenStorageTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString oTitle = "Existing Title";
    const QString oDescription = "Existing Description";
    const QStringList oKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.getKeywords(), oKeywords);
    QCOMPARE(metadata.isModified(), false);

    const QString cTitle = "Other Title";
    const QString cDescription = "other Description";
    const QStringList cKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.getKeywords(), oKeywords + cKeywords);
    QCOMPARE(metadata.isModified(), true);
}

void ArtworkMetadataTests::initStorageThenOriginTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString cTitle = "Existing Title";
    const QString cDescription = "Existing Description";
    const QStringList cKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isAlmostInitialized(), true);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.getTitle(), cTitle);
    QCOMPARE(metadata.getDescription(), cDescription);
    QCOMPARE(metadata.getKeywords(), cKeywords);
    QCOMPARE(metadata.isModified(), false);

    const QString oTitle = "Other Title";
    const QString oDescription = "other Description";
    const QStringList oKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.getKeywords(), oKeywords);
    QCOMPARE(metadata.isModified(), false);
}

void ArtworkMetadataTests::initOriginalThenStorageDescriptionEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString oTitle = "Existing Title";
    const QString oDescription = "";
    const QStringList oKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.isModified(), false);

    const QString cTitle = "Other Title";
    const QString cDescription = "other Description";
    const QStringList cKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), cDescription);
    QCOMPARE(metadata.isModified(), true);
}

void ArtworkMetadataTests::initOriginalThenStorageTitleEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString oTitle = "";
    const QString oDescription = "Existing Description";
    const QStringList oKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), oTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.isModified(), false);

    const QString cTitle = "Other Title";
    const QString cDescription = "other Description";
    const QStringList cKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), cTitle);
    QCOMPARE(metadata.getDescription(), oDescription);
    QCOMPARE(metadata.isModified(), true);
}

void ArtworkMetadataTests::initStorageThenOriginKeywordsOverwriteTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString cTitle = "Existing Title";
    const QString cDescription = "Existing Description";
    const QStringList cKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isAlmostInitialized(), true);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.getKeywords(), cKeywords);
    QCOMPARE(metadata.isModified(), false);

    const QString oTitle = "Other Title";
    const QString oDescription = "other Description";
    const QStringList oKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getKeywords(), oKeywords);
    QCOMPARE(metadata.isModified(), false);
}

void ArtworkMetadataTests::initStorageThenOriginKeywordsAddTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString cTitle = "Existing Title";
    const QString cDescription = "Existing Description";
    const QStringList cKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2" << "keyword4" << "keyword5" << "keyword6";
    bool result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.isAlmostInitialized(), true);
    QCOMPARE(metadata.getKeywords(), cKeywords);
    QCOMPARE(metadata.isModified(), false);

    const QString oTitle = "Other Title";
    const QString oDescription = "other Description";
    const QStringList oKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getKeywords(), cKeywords);
    QCOMPARE(metadata.isModified(), true);
}

void ArtworkMetadataTests::initStorageThenOriginShouldModifyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");

    const QString cTitle = "Existing Title";
    const QString cDescription = "Existing Description";
    const QStringList cKeywords = QStringList() << "keyword1" << "keyword3" << "keyword2";
    bool result = metadata.initFromStorage(CA(cTitle, cDescription, cKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.isAlmostInitialized(), true);
    QCOMPARE(metadata.getTitle(), cTitle);
    QCOMPARE(metadata.getDescription(), cDescription);
    QCOMPARE(metadata.getKeywords(), cKeywords);
    QCOMPARE(metadata.isModified(), false);

    const QString oTitle = "";
    const QString oDescription = "";
    const QStringList oKeywords = QStringList() << "keyword4" << "keyword5" << "keyword6";
    result = metadata.initFromOrigin(OM(oTitle, oDescription, oKeywords));
    QCOMPARE(result, true);
    QCOMPARE(metadata.isInitialized(), true);
    QCOMPARE(metadata.getTitle(), cTitle);
    QCOMPARE(metadata.getDescription(), cDescription);
    QCOMPARE(metadata.getKeywords(), oKeywords);
    QCOMPARE(metadata.isModified(), true);
}

#define PARALLEL_TEST_BEGIN \
    const int initIterations = 10000; \
    Mocks::ArtworkMetadataMock metadata("file.jpg"); \
    MetadataIO::OriginalMetadata om; \
    om.m_Keywords = QStringList() << "keyword1" << "keyword2" << "keyword3"; \
    om.m_Title = "titleOM"; \
    om.m_Description = "description OM"; \
    MetadataIO::CachedArtwork ca; \
    ca.m_Keywords = QStringList() << "keyword8" << "keyword7" << "keyword6"; \
    ca.m_Title = "title CA"; \
    ca.m_Description = "descriptionCA";

void ArtworkMetadataTests::parallelInitOriginStorageTest() {
    PARALLEL_TEST_BEGIN;

    Helpers::Barrier barrier1(2), barrier2(2);

    std::thread originThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.initFromOrigin(om, false);

            /*-------------*/barrier1.wait();

            metadata.resetAll();

            /*-------------*/barrier2.wait();
        }
    });

    std::thread cachedThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.initFromStorage(ca);

            /*-------------*/barrier1.wait();

            // metadata.resetAll(); // anough 1 reset in the other thread

            /*-------------*/barrier2.wait();
        }
    });

    originThread.join();
    cachedThread.join();
}

void ArtworkMetadataTests::parallelInitEmptyStorageTest() {
    PARALLEL_TEST_BEGIN;

    Helpers::Barrier barrier1(2), barrier2(2);

    std::thread originThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.initAsEmpty();

            /*-------------*/barrier1.wait();

            metadata.resetAll();

            /*-------------*/barrier2.wait();
        }
    });

    std::thread cachedThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.initFromStorage(ca);

            /*-------------*/barrier1.wait();

            // metadata.resetAll(); // anough 1 reset in the other thread

            /*-------------*/barrier2.wait();
        }
    });

    originThread.join();
    cachedThread.join();
}

void ArtworkMetadataTests::parallelInitStorageSetTest() {
    PARALLEL_TEST_BEGIN;

    Helpers::Barrier barrier2(2), barrier3(2);

    std::thread originThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.setTitle("test");
            metadata.setDescription("another test");
            metadata.setKeywords(QStringList() << "some brand new keyword");

            /*-------------*/barrier2.wait();

            metadata.resetAll();

            /*-------------*/barrier3.wait();
        }
    });

    std::thread cachedThread([&]() {
        int n = initIterations;
        while (n--) {
            /*-------------*/barrier2.wait();

            // metadata.resetAll(); // anough 1 reset in the other thread

            /*-------------*/barrier3.wait();
        }
    });

    originThread.join();
    cachedThread.join();
}

void ArtworkMetadataTests::parallelInitOriginSetTest() {
    PARALLEL_TEST_BEGIN;

    Helpers::Barrier barrier2(2), barrier3(2);

    std::thread originThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.setTitle("test");
            metadata.setDescription("another test");
            metadata.setKeywords(QStringList() << "some brand new keyword");

            /*-------------*/barrier2.wait();

            metadata.resetAll();

            /*-------------*/barrier3.wait();
        }
    });

    std::thread cachedThread([&]() {
        int n = initIterations;
        while (n--) {
            metadata.initFromOrigin(om);

            /*-------------*/barrier2.wait();

            // metadata.resetAll(); // anough 1 reset in the other thread

            /*-------------*/barrier3.wait();
        }
    });

    originThread.join();
    cachedThread.join();
}

void ArtworkMetadataTests::freshObjectTest() {
    QString filepath = "/path/to/my/file.jpg";
    Mocks::ArtworkMetadataMock metadata(filepath);

    QCOMPARE(metadata.getFilepath(), filepath);
    QCOMPARE(metadata.isInitialized(), false);
    QCOMPARE(metadata.isDescriptionEmpty(), true);
    QCOMPARE(metadata.isTitleEmpty(), true);
    QCOMPARE(metadata.isModified(), false);
    QCOMPARE(metadata.isEmpty(), true);
    QCOMPARE(metadata.isSelected(), false);
}

void ArtworkMetadataTests::markModifiedShouldEmitSignalTest() {
     Mocks::ArtworkMetadataMock metadata("file.jpg");

     QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

     metadata.markModified();

     QCOMPARE(modifiedSpy.count(), 1);
     QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
     QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::modifiedIsNotMarkedModifiedAgainTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.markModified();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.markModified();

    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::setDescriptionEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setDescription("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::setTitleEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.setTitle("Not empty value");

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::addNewKeywordsEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    int addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 2);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(addedSpy.count(), 1);
    QList<QVariant> addedArguments = addedSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 1);
}

void ArtworkMetadataTests::addExistingKeywordsDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();
    int addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 2);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    addedCount = metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QCOMPARE(addedCount, 0);
    QCOMPARE(metadata.rowCount(), 2);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::addOneNewKeywordEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    bool added = metadata.appendKeyword("keyword1");

    QCOMPARE(added, true);
    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(addedSpy.count(), 1);
    QList<QVariant> addedArguments = addedSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::addOneExistingKeywordDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    bool added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, true);

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy addedSpy(metadata.getBasicModel(), SIGNAL(rowsInserted(QModelIndex,int,int)));

    added = metadata.appendKeyword("keyword2");

    QCOMPARE(added, false);
    QCOMPARE(metadata.rowCount(), 1);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
}

void ArtworkMetadataTests::removeKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString keyword;
    bool removed = metadata.removeKeywordAt(0, keyword);

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeLastKeywordFromEmptyTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString keyword;
    bool removed = metadata.removeLastKeyword(keyword);

    QCOMPARE(removed, false);
    QCOMPARE(modifiedSpy.count(), 0);
    QCOMPARE(removedSpy.count(), 0);
}

void ArtworkMetadataTests::removeActualKeywordTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    QString keyword;
    bool removed = metadata.removeKeywordAt(0, keyword);

    QCOMPARE(removed, true);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> removeArguments = removedSpy.takeFirst();
    QCOMPARE(removeArguments.at(1).toInt(), 0);
    QCOMPARE(removeArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::removeLastActualKeywordTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));
    QSignalSpy removedSpy(metadata.getBasicModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)));

    metadata.appendKeyword("keyword1");

    QString keyword;
    bool removed = metadata.removeLastKeyword(keyword);

    QCOMPARE(removed, true);

    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);

    QCOMPARE(removedSpy.count(), 1);
    QList<QVariant> removeArguments = removedSpy.takeFirst();
    QCOMPARE(removeArguments.at(1).toInt(), 0);
    QCOMPARE(removeArguments.at(2).toInt(), 0);
}

void ArtworkMetadataTests::editKeywordToAnotherEmitsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    bool editResult = metadata.editKeyword(0, "another");

    QCOMPARE(editResult, true);
    QCOMPARE(modifiedSpy.count(), 1);
    QList<QVariant> modifyArguments = modifiedSpy.takeFirst();
    QCOMPARE(modifyArguments.at(0).toBool(), true);
}

void ArtworkMetadataTests::editKeywordToExistingDoesNotEmitModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    bool editResult = metadata.editKeyword(0, "keyword2");

    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::misEditOfKeywordDoesNothingTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");

    QSignalSpy modifiedSpy(&metadata, SIGNAL(modifiedChanged(bool)));

    bool editResult = metadata.editKeyword(2, "keyword2");

    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void ArtworkMetadataTests::isInDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString alsoDir = "C:\\path\\to\\directory\\of";
#else
    QString dir = "/path/to/directory/of/";
    QString alsoDir = "/path/to/directory/of";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    qDebug() << QDir(dir).absolutePath();
    QVERIFY(metadata.isInDirectory(QDir(dir).absolutePath()));
    QVERIFY(metadata.isInDirectory(QDir(alsoDir).absolutePath()));
}

void ArtworkMetadataTests::isNotInParentsDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString notADir1 = "C:\\path\\to\\directory";
    QString notADir2 = "C:\\path\\to\\directory\\";
    QString notADir3 = "C:\\path\\to\\";
#else
    QString dir = "/path/to/directory/of/";
    QString notADir1 = "/path/to/directory/";
    QString notADir2 = "/path/to/directory";
    QString notADir3 = "/path/to/";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());

    QVERIFY(!metadata.isInDirectory(QDir(notADir1).absolutePath()));
    QVERIFY(!metadata.isInDirectory(QDir(notADir2).absolutePath()));
    QVERIFY(!metadata.isInDirectory(QDir(notADir3).absolutePath()));
}

void ArtworkMetadataTests::isNotInOtherDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString otherDir = "C:\\path\\to\\some\\other\\directory\\of\\";
#else
    QString dir = "/path/to/directory/of/";
    QString otherDir = "/path/to/some/other/directory/of/";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    QVERIFY(!metadata.isInDirectory(QDir(otherDir).absolutePath()));
}

void ArtworkMetadataTests::isNotInEmptyDirectoryTest() {
    QString filename = "file.jpg";
#ifdef Q_OS_WIN
    QString dir = "C:\\path\\to\\directory\\of\\";
    QString otherDir = "";
#else
    QString dir = "/path/to/directory/of/";
    QString otherDir = "";
#endif

    QFileInfo info(dir + filename);
    Mocks::ArtworkMetadataMock metadata(info.absoluteFilePath());
    QVERIFY(!metadata.isInDirectory(QDir(otherDir).absolutePath()));
}

void ArtworkMetadataTests::clearKeywordsMarksAsModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    metadata.resetModified();

    metadata.clearKeywords();
    QVERIFY(metadata.isModified());
}

void ArtworkMetadataTests::clearEmptyKeywordsDoesNotMarkModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    QVERIFY(!metadata.isModified());

    metadata.clearKeywords();
    QVERIFY(!metadata.isModified());
}

void ArtworkMetadataTests::removeKeywordsMarksModifiedTest() {
    Mocks::ArtworkMetadataMock metadata("file.jpg");
    metadata.initAsEmpty();
    metadata.appendKeywords(QStringList() << "keyword1" << "keyword2");
    metadata.resetModified();

    bool result = metadata.removeKeywords(QSet<QString>() << "keyword3");
    QVERIFY(!result);
    QVERIFY(!metadata.isModified());

    result = metadata.removeKeywords(QSet<QString>() << "keyword2");
    QVERIFY(result);
    QVERIFY(metadata.isModified());
}
