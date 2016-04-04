#include "conectivityhelpers_tests.h"
#include "../../xpiks-qt/Conectivity/conectivityhelpers.h"
#include "Mocks/artworkmetadatamock.h"

#ifdef Q_OS_WIN
#define FIX_PATH(x) "C:"x
#else
#define FIX_PATH(x) (x)
#endif

void ConectivityHelpersTests::generateMixedFilepathesTest() {
    Mocks::ArtworkMetadataMock metadataWithoutVector(FIX_PATH("/path/to/picture.jpg"));
    Mocks::ArtworkMetadataMock metadataWithVector(FIX_PATH("/completely/another/path/to/picture.jpg"));
    metadataWithVector.attachVector(FIX_PATH("/completely/another/path/to/picture.eps"));
    QVector<Models::ArtworkMetadata*> items;
    items << &metadataWithoutVector << &metadataWithVector;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << FIX_PATH("/path/to/picture.jpg")
                  << FIX_PATH("/completely/another/path/to/picture.jpg")
                  << FIX_PATH("/completely/another/path/to/picture.eps");

    expectedZips << FIX_PATH("/path/to/picture.jpg")
                 << FIX_PATH("/completely/another/path/to/picture.zip");

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}

void ConectivityHelpersTests::generateWithoutVectorsTest() {
    Mocks::ArtworkMetadataMock metadata1(FIX_PATH("/path/to/picture.jpg"));
    Mocks::ArtworkMetadataMock metadata2(FIX_PATH("/completely/another/path/to/picture.jpg"));
    QVector<Models::ArtworkMetadata*> items;
    items << &metadata1 << &metadata2;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << FIX_PATH("/path/to/picture.jpg")
                  << FIX_PATH("/completely/another/path/to/picture.jpg");

    expectedZips << FIX_PATH("/path/to/picture.jpg")
                 << FIX_PATH("/completely/another/path/to/picture.jpg");

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}

void ConectivityHelpersTests::generateWithVectorsTest() {
    Mocks::ArtworkMetadataMock metadataWithVector1(FIX_PATH("/path/to/picture.jpg"));
    metadataWithVector1.attachVector(FIX_PATH("/path/to/picture.eps"));
    Mocks::ArtworkMetadataMock metadataWithVector2(FIX_PATH("/completely/another/path/to/picture.jpg"));
    metadataWithVector2.attachVector(FIX_PATH("/completely/another/path/to/picture.eps"));
    QVector<Models::ArtworkMetadata*> items;
    items << &metadataWithVector1 << &metadataWithVector2;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << FIX_PATH("/path/to/picture.jpg")
                  << FIX_PATH("/path/to/picture.eps")
                  << FIX_PATH("/completely/another/path/to/picture.jpg")
                  << FIX_PATH("/completely/another/path/to/picture.eps");

    expectedZips << FIX_PATH("/path/to/picture.zip")
                 << FIX_PATH("/completely/another/path/to/picture.zip");

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}
