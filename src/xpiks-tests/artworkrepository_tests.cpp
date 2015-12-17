#include "artworkrepository_tests.h"
#include "../xpiks-qt/Models/artworksrepository.h"

void ArtworkRepositoryTests::simpleAccountFileTest() {
    Models::ArtworksRepository repository;

    bool status = repository.accountFile("/path/to/some/file");

    QCOMPARE(status, true);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getDirectory(0), QLatin1String("/path/to/some"));
    QCOMPARE(repository.getFilesCountForDirectory(QLatin1String("/path/to/some")), 1);
}

void ArtworkRepositoryTests::accountSameFileTest() {
    Models::ArtworksRepository repository;

    repository.accountFile("/path/to/some/file");
    bool status = repository.accountFile("/path/to/some/file");

    QCOMPARE(status, false);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getDirectory(0), QLatin1String("/path/to/some"));
    QCOMPARE(repository.getFilesCountForDirectory(QLatin1String("/path/to/some")), 1);
}

void ArtworkRepositoryTests::addFilesFromOneDirectoryTest() {
    Models::ArtworksRepository repository;

    bool anyWrong = false;
    int count = 5;
    while (count--) {
        QString filename = QString("/path/to/some/file%1.jpg").arg(5 - count - 1);
        if (!repository.accountFile(filename)) {
            anyWrong = true;
        }
    }

    QCOMPARE(anyWrong, false);
    QCOMPARE(repository.getArtworksSourcesCount(), 1);
    QCOMPARE(repository.getFilesCountForDirectory("/path/to/some"), 5);
}
