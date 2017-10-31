#include "faileduploadstest.h"
#include <QDebug>
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artworkuploader.h"
#include "../../xpiks-qt/Models/uploadinforepository.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/uploadinfo.h"
#include "../../xpiks-qt/Connectivity/uploadwatcher.h"
#include "signalwaiter.h"
#include "testshelpers.h"

QString FailedUploadsTest::testName() {
    return QLatin1String("FailedUploadsTest");
}

void FailedUploadsTest::setup() {
    Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();
    settingsModel->setAutoFindVectors(false);
}

int FailedUploadsTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/vector/026.jpg");

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

    const QString filepath = artItemsModel->getArtworkFilepath(0);

    Models::FilteredArtItemsProxyModel *filteredModel = m_CommandManager->getFilteredArtItemsModel();
    filteredModel->selectFilteredArtworks();
    filteredModel->setSelectedForUpload();

    Models::UploadInfoRepository *uploadRepo = m_CommandManager->getUploadInfoRepository();

    auto remote1 = uploadRepo->appendItem();
    const QString host1 = "ftp://random.host.com/";
    remote1->setHost(host1);
    remote1->setUsername("john");
    remote1->setPassword("doe");
    remote1->setIsSelected(true);

    auto remote2 = uploadRepo->appendItem();
    const QString host2 = "ftp://another.host.com/";
    remote2->setHost(host2);
    remote2->setUsername("doe");
    remote2->setPassword("john");
    remote2->setIsSelected(true);

    Models::ArtworkUploader *uploader = m_CommandManager->getArtworkUploader();
    uploader->uploadArtworks();

    sleepWaitUntil(10, [&uploader]() {
        return uploader->getInProgress() == false;
    });

    VERIFY(uploader->getInProgress() == false, "Uploader is still in progress");
    QCoreApplication::processEvents();

    Connectivity::UploadWatcher *watcher = uploader->accessWatcher();
    auto &failedUploads = watcher->getFailedUploads();
    qDebug() << "Failed images count is" << watcher->getFailedImagesCount();

    VERIFY(watcher->getFailedImagesCount() == 2, "Wrong failed images count");
    VERIFY(failedUploads.size() == 2, "Both hosts did not fail");

    for (auto &pair: failedUploads) {
        auto failedFiles = pair.second;
        VERIFY(failedFiles.size() == 1, "Fail Info contains more than 1 file");
        VERIFY(failedFiles[0] == filepath, "Fail Info contains wrong file");
        VERIFY((pair.first == host1) || (pair.first == host2), "Wrong host in Failed Info");
    }

    return 0;
}
