#include "savevideobasictest.h"
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
#include "../../xpiks-qt/Models/videoartwork.h"

QString SaveVideoBasicTest::testName() {
    return QLatin1String("SaveVideoBasicTest");
}

void SaveVideoBasicTest::setup() {
}

int SaveVideoBasicTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("videos-for-tests/Untitled.mp4");

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
    const Common::ID_t id = artwork->getItemID();
    Models::VideoArtwork *video = dynamic_cast<Models::VideoArtwork*>(artwork);

    VERIFY(video->getImageSize().width() == 640, "Video width was read incorrectly");
    VERIFY(video->getImageSize().height() == 400, "Video height was read incorrectly");

    QStringList keywords; keywords << "untitled" << "first" << "video";
    QString title = "Title for the video";
    QString description = "Description for the video";
    artwork->setDescription(description);
    artwork->setTitle(title);
    artwork->getBasicModel()->setKeywords(keywords);
    artwork->setIsSelected(true);

    bool doOverwrite = true, dontSaveBackups = false;

    QObject::connect(ioCoordinator, SIGNAL(metadataWritingFinished()), &waiter, SIGNAL(finished()));
    artItemsModel->saveSelectedArtworks(QVector<int>() << 0, doOverwrite, dontSaveBackups);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for writing metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while writing");

    artItemsModel->removeSelectedArtworks(QVector<int>() << 0);

    addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == 1, "Failed to add file");

    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    artwork = artItemsModel->getArtwork(0);
    const QStringList &actualKeywords = artwork->getKeywords();
    const QString &actualTitle = artwork->getTitle();
    const QString &actualDescription = artwork->getDescription();

    VERIFY(id != artwork->getItemID(), "ID should not match");
    VERIFY(actualKeywords == keywords, "Read keywords are not the same");
    VERIFY(actualTitle == title, "Real title is not the same");
    VERIFY(actualDescription == description, "Real description is not the same");

    return 0;
}

