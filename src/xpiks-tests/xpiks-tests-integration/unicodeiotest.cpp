#include "unicodeiotest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "exiv2iohelpers.h"

QString UnicodeIoTest::testName() {
    return QLatin1String("UnicodeIoTest");
}

void UnicodeIoTest::setup() {
}

int UnicodeIoTest::doTest() {
    // reading part ---------------------------------

    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/utf8-meta/Eiffel_Wikimedia_Commons.jpg");

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

    Models::ArtworkMetadata *artwork = artItemsModel->getArtwork(0);

    BasicMetadata basicMetadata;
    ::readMetadata(artwork->getFilepath(), basicMetadata);

    VERIFY(basicMetadata.m_Description == artwork->getDescription(), "Description does not match for reading");
    VERIFY(basicMetadata.m_Title == artwork->getTitle(), "Title does not match for reading")
    VERIFY(basicMetadata.m_Keywords == artwork->getKeywords(), "Keywords do not match for reading");

    // writing part ---------------------------------

    files.clear();
    files << getFilePathForTest("images-for-tests/utf8-meta/Maurice_koechlin_pylone.jpg");

    addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == files.length(), "Failed to add file");
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    artwork = artItemsModel->getArtwork(1);

    const QString titleu8 = QStringLiteral("πύργος του Άιφελ");
    const QString description8u = QStringLiteral("První plány stavby byly zahájeny už v roce 1878.");
    QString keywordsStr = QStringLiteral("buokšts, sėmbuolu, Parīžiuo, aukštliausės, bodīnks, metās, Е́йфелева, ве́жа, 埃菲尔铁塔");
    const QStringList keywordsu8 = keywordsStr.split(", ");

    artwork->setTitle(titleu8);
    artwork->setDescription(description8u);
    artwork->setKeywords(keywordsu8);
    artwork->setIsSelected(true);

    bool doOverwrite = true, dontSaveBackups = false;

    QObject::connect(ioCoordinator, SIGNAL(metadataWritingFinished()), &waiter, SIGNAL(finished()));
    artItemsModel->saveSelectedArtworks(QVector<int>() << 1, doOverwrite, dontSaveBackups);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for writing metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while writing");

    BasicMetadata exifMetadata, iptcMetadata, xmpMetadata;
    ::readMetadataEx(artwork->getFilepath(), xmpMetadata, exifMetadata, iptcMetadata);

    VERIFY(exifMetadata.m_Description == description8u, "Exif description does not match");

    VERIFY(iptcMetadata.m_Title == titleu8, "IPTC title does not match")
    VERIFY(iptcMetadata.m_Description == description8u, "IPTC description does not match");
    VERIFY(iptcMetadata.m_Keywords == keywordsu8, "IPTC keyowrds do not match");

    VERIFY(xmpMetadata.m_Title == titleu8, "XMP title does not match");
    VERIFY(xmpMetadata.m_Description == description8u, "XMP description does not match");
    VERIFY(xmpMetadata.m_Keywords == keywordsu8, "XMP keywords do not match");

    return 0;
}

