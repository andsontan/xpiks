#include "artworkrepository_tests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/Models/artworksrepository.h"

void ArtworkRepositoryTests::simpleAccountFileTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
    QString directory = "C:/path/to/some";
#else
    QString filename = "/path/to/some/file";
    QString directory = "/path/to/some";
#endif

    bool status = repository.accountFile(filename);

    QCOMPARE(status, true);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getDirectory(0), directory);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 1);
}

void ArtworkRepositoryTests::accountSameFileTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
    QString directory = "C:/path/to/some";
#else
    QString filename = "/path/to/some/file";
    QString directory = "/path/to/some";
#endif

    repository.accountFile(filename);
    bool status = repository.accountFile(filename);

    QCOMPARE(status, false);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getDirectory(0), directory);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 1);
}

void ArtworkRepositoryTests::addFilesFromOneDirectoryTest() {
    Models::ArtworksRepository repository;

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
        if (!repository.accountFile(filename)) {
            anyWrong = true;
        }
    }

    QCOMPARE(anyWrong, false);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getFilesCountForDirectory(directory), 5);
}

void ArtworkRepositoryTests::addAndRemoveSameFileTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename = "C:/path/to/some/file";
    QString directory = "C:/path/to/some";
#else
    QString filename = "/path/to/some/file";
    QString directory = "/path/to/some";
#endif

    bool status = repository.accountFile(filename);
    QCOMPARE(status, true);

    bool removeResult = repository.removeFile(filename, directory);
    repository.cleanupEmptyDirectories();

    QCOMPARE(repository.getArtworksSourcesCount(), 0);
    QCOMPARE(removeResult, true);
}

void ArtworkRepositoryTests::removeNotExistingFileTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
    QString directory = "C:/path/to/some";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
    QString directory = "/path/to/some";
#endif

    bool status = repository.accountFile(filename1);
    QCOMPARE(status, true);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);

    bool removeResult = repository.removeFile(filename2, directory);
    repository.cleanupEmptyDirectories();

    QCOMPARE(removeResult, false);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
}

void ArtworkRepositoryTests::brandNewDirectoriesCountTest() {
    Models::ArtworksRepository repository;

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
    QCOMPARE(repository.getArtworksSourcesCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::differentNewDirectoriesCountTest() {
    Models::ArtworksRepository repository;

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
    QCOMPARE(repository.getArtworksSourcesCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::newFilesCountTest() {
    Models::ArtworksRepository repository;

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
    QCOMPARE(repository.getArtworksSourcesCount(), 0);
    QCOMPARE(repository.rowCount(), 0);
}

void ArtworkRepositoryTests::noNewDirectoriesCountTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    repository.accountFile(filename1);

    QStringList files;
    files << filename2;

    int newFilesCount = repository.getNewDirectoriesCount(files);
    QCOMPARE(newFilesCount, 0);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
}

void ArtworkRepositoryTests::noNewFilesCountTest() {
    Models::ArtworksRepository repository;

#ifdef Q_OS_WIN
    QString filename1 = "C:/path/to/some/file1";
    QString filename2 = "C:/path/to/some/file2";
#else
    QString filename1 = "/path/to/some/file1";
    QString filename2 = "/path/to/some/file2";
#endif

    QStringList files;
    files << filename1 << filename2;

    foreach (const QString &file, files) {
        repository.accountFile(file);
    }

    int newFilesCount = repository.getNewFilesCount(files);
    QCOMPARE(newFilesCount, 0);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
}

void ArtworkRepositoryTests::endAccountingWithNoNewFilesTest() {
    Models::ArtworksRepository repository;
    QSignalSpy endSpy(&repository, SIGNAL(rowsInserted(QModelIndex,int,int)));

    repository.endAccountingFiles(false);
    QVERIFY(endSpy.isEmpty());
}

void ArtworkRepositoryTests::startAccountingNewFilesEmitsTest() {
    Models::ArtworksRepository repository;

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
