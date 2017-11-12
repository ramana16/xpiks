#include "artworkrepository_tests.h"
#include <QSignalSpy>
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/artworksrepository.h"
#include "../../xpiks-qt/QuickBuffer/quickbuffer.h"
#include "../../xpiks-qt/KeywordsPresets/presetkeywordsmodel.h"
#include "../../xpiks-qt/Models/artworkproxymodel.h"

#define DECLARE_MODELS_AND_GENERATE(count, withVector) \
    Mocks::CommandManagerMock commandManagerMock;\
    Mocks::ArtItemsModelMock artItemsModelMock;\
    Models::ArtworksRepository artworksRepository;\
    Models::FilteredArtItemsProxyModel filteredItemsModel;\
    commandManagerMock.InjectDependency(&artworksRepository);\
    commandManagerMock.InjectDependency(&artItemsModelMock);\
    filteredItemsModel.setSourceModel(&artItemsModelMock);\
    commandManagerMock.InjectDependency(&filteredItemsModel);\
    commandManagerMock.generateAndAddArtworks(count, withVector);

void ArtworkRepositoryTests::simpleAccountFileTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
    QString directory = "C:/path/to/some";
#else
    QString filename = "/path/to/some/file";
    QString directory = "/path/to/some";
#endif
    qint64 dirID = 0;
    bool status = repository.accountFile(filename, dirID);

    QCOMPARE(status, true);
    QCOMPARE(repository.rowCount(), 1);
    QCOMPARE(repository.getDirectoryPath(0), directory);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 1);
}

void ArtworkRepositoryTests::accountSameFileTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);


#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
    QString directory = "C:/path/to/some";
#else
    QString filename = "/path/to/some/file";
    QString directory = "/path/to/some";
#endif

    qint64 dirID = 0;
    repository.accountFile(filename, dirID);
    bool status = repository.accountFile(filename, dirID);

    QCOMPARE(status, false);
    QCOMPARE(repository.rowCount(), 1);
    QCOMPARE(repository.getDirectoryPath(0), directory);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 1);
}

void ArtworkRepositoryTests::addFilesFromOneDirectoryTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);;

#ifdef Q_OS_WIN
    QString filenameTemplate = "C:/path/to/some/file%1.jpg";
    QString directory = "C:/path/to/some";
#else
    QString filenameTemplate = "/path/to/some/file%1.jpg";
    QString directory = "/path/to/some";
    #endif

    bool anyWrong = false;
    int count = 5;
    while (count--) {
        QString filename = filenameTemplate.arg(5 - count - 1);
        qint64 dirID = 0;
        if (!repository.accountFile(filename, dirID)) {
            anyWrong = true;
        }
    }

    QCOMPARE(anyWrong, false);
    QCOMPARE(repository.rowCount(), 1);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 5);
}

void ArtworkRepositoryTests::addAndRemoveSameFileTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
#else
    QString filename = "/path/to/some/file";
#endif

    qint64 dirID = 0;
    bool status = repository.accountFile(filename, dirID);
    QCOMPARE(status, true);

    bool removeResult = repository.removeFile(filename, dirID);
    repository.cleanupEmptyDirectories();

    QCOMPARE(repository.rowCount(), 0);
    QCOMPARE(removeResult, true);
}

void ArtworkRepositoryTests::removeNotExistingFileTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    qint64 dirID = 0;
    bool status = repository.accountFile(filename1, dirID);
    QCOMPARE(status, true);
    QCOMPARE(repository.rowCount(), 1);

    bool removeResult = repository.removeFile(filename2, dirID);
    repository.cleanupEmptyDirectories();

    QCOMPARE(removeResult, false);
    QCOMPARE(repository.rowCount(), 1);
}

void ArtworkRepositoryTests::brandNewDirectoriesCountTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    int newDirsCount = repository.getNewDirectoriesCount(files);
    QCOMPARE(newDirsCount, 1);
    QCOMPARE(repository.rowCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::differentNewDirectoriesCountTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/other/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/other/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    int newDirsCount = repository.getNewDirectoriesCount(files);
    QCOMPARE(newDirsCount, files.length());
    QCOMPARE(repository.rowCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::newFilesCountTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    int newFilesCount = repository.getNewFilesCount(files);
    QCOMPARE(newFilesCount, files.length());
    QCOMPARE(repository.rowCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::noNewDirectoriesCountTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    qint64 dirID = 0;
    repository.accountFile(filename1, dirID);

    QStringList files;
    files << filename2;

    int newFilesCount = repository.getNewDirectoriesCount(files);
    QCOMPARE(newFilesCount, 0);
    QCOMPARE(repository.rowCount(), 1);
}

void ArtworkRepositoryTests::noNewFilesCountTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    qint64 dirID = 0;
    foreach (const QString &file, files) {
        repository.accountFile(file, dirID);
    }

    int newFilesCount = repository.getNewFilesCount(files);
    QCOMPARE(newFilesCount, 0);
    QCOMPARE(repository.rowCount(), 1);
}

void ArtworkRepositoryTests::endAccountingWithNoNewFilesTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);
    QSignalSpy endSpy(&repository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    repository.endAccountingFiles(false);
    QVERIFY(endSpy.isEmpty());
}

void ArtworkRepositoryTests::startAccountingNewFilesEmitsTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/other/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/other/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    QSignalSpy beginSpy(&repository, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    bool newFiles = repository.beginAccountingFiles(files);

    QCOMPARE(newFiles, true);
    QCOMPARE(beginSpy.count(), 1);
    QList<QVariant> addArguments = beginSpy.takeFirst();
    QCOMPARE(addArguments.at(1).toInt(), 0);
    QCOMPARE(addArguments.at(2).toInt(), files.length() - 1);
}

void ArtworkRepositoryTests::selectFolderTest() {
    Mocks::CommandManagerMock commandManagerMock;
    Models::ArtworksRepository repository;
    commandManagerMock.InjectDependency(&repository);

#ifdef Q_OS_WIN
    QString filename1 = "C:/path1/to/some/file";
    QString filename2 = "C:/path2/to/some/file";
    QString filename3 = "C:/path3/to/some/file";
    QString filename4 = "C:/path4/to/some/file";
#else
    QString filename1 = "/path1/to/some/file";
    QString filename2 = "/path2/to/some/file";
    QString filename3 = "/path3/to/some/file";
    QString filename4 = "/path4/to/some/file";
#endif

    QStringList files;
    files << filename1 << filename2 << filename3;

    std::vector<qint64> dirIDs;
    foreach (const QString &file, files) {
        qint64 dirID;
        repository.accountFile(file, dirID);
        dirIDs.push_back(dirID);
    }

    // Initially all directories are selected.
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), true);

    //If All are selected and you click on 1, you select it and deselect others.
    repository.toggleDirectorySelected(0);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), false);

    //If not all are selected and you click on 1, you add it to the selection.
    repository.toggleDirectorySelected(2);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), true);

    repository.toggleDirectorySelected(0);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), true);

    //If you unselect last selected, all get selected.
    repository.toggleDirectorySelected(2);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), true);

    repository.toggleDirectorySelected(2);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), false);
    QCOMPARE(repository.isDirectorySelected(dirIDs[2]), true);

    //If you remove last selected directory, all get selected.
    repository.removeItem(2);
    QCOMPARE(repository.isDirectorySelected(dirIDs[0]), true);
    QCOMPARE(repository.isDirectorySelected(dirIDs[1]), true);

    //If you add a new directory, it gets selected by default.
    qint64 dirID;
    repository.accountFile(filename4, dirID);
    dirIDs.push_back(dirID);
    QCOMPARE(repository.isDirectorySelected(dirIDs[3]), true);
}

void ArtworkRepositoryTests::oneEmptyDirectoryStaysTest() {
    const int count = 1;
    DECLARE_MODELS_AND_GENERATE(count, false);

    QCOMPARE(artworksRepository.getFilesCountForDirectory(0), 1);
    QCOMPARE(artItemsModelMock.getArtworksCount(), count);

    artItemsModelMock.removeArtworksDirectory(0);
    QCOMPARE(artworksRepository.rowCount(), 1);

    artworksRepository.cleanupEmptyDirectories();
    QCOMPARE(artworksRepository.rowCount(), 0);
}

void ArtworkRepositoryTests::fewEmptyDirectoriesStayTest() {
    const int count = 2;
    DECLARE_MODELS_AND_GENERATE(count, false);

    QCOMPARE(artworksRepository.getFilesCountForDirectory(0), 1);
    QCOMPARE(artworksRepository.getFilesCountForDirectory(1), 1);
    QCOMPARE(artItemsModelMock.getArtworksCount(), count);

    artItemsModelMock.removeArtworksDirectory(0);
    artItemsModelMock.removeArtworksDirectory(1);
    QCOMPARE(artworksRepository.rowCount(), 2);

    artworksRepository.cleanupEmptyDirectories();
    QCOMPARE(artworksRepository.rowCount(), 0);
}
