#include "conectivityhelpers_tests.h"
#include "../xpiks-qt/Conectivity/conectivityhelpers.h"
#include "Mocks/artworkmetadatamock.h"

void ConectivityHelpersTests::generateMixedFilepathesTest() {
    Mocks::ArtworkMetadataMock metadataWithoutVector("/path/to/picture.jpg");
    Mocks::ArtworkMetadataMock metadataWithVector("/completely/another/path/to/picture.jpg");
    metadataWithVector.attachVector("/completely/another/path/to/picture.eps");
    QVector<Models::ArtworkMetadata*> items;
    items << &metadataWithoutVector << &metadataWithVector;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << "/path/to/picture.jpg"
                  << "/completely/another/path/to/picture.jpg"
                  << "/completely/another/path/to/picture.eps";

    expectedZips << "/path/to/picture.jpg"
                 << "/completely/another/path/to/picture.zip";

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}

void ConectivityHelpersTests::generateWithoutVectorsTest() {
    Mocks::ArtworkMetadataMock metadata1("/path/to/picture.jpg");
    Mocks::ArtworkMetadataMock metadata2("/completely/another/path/to/picture.jpg");
    QVector<Models::ArtworkMetadata*> items;
    items << &metadata1 << &metadata2;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << "/path/to/picture.jpg"
                  << "/completely/another/path/to/picture.jpg";

    expectedZips << "/path/to/picture.jpg"
                 << "/completely/another/path/to/picture.jpg";

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}

void ConectivityHelpersTests::generateWithVectorsTest() {
    Mocks::ArtworkMetadataMock metadataWithVector1("/path/to/picture.jpg");
    metadataWithVector1.attachVector("/path/to/picture.eps");
    Mocks::ArtworkMetadataMock metadataWithVector2("/completely/another/path/to/picture.jpg");
    metadataWithVector2.attachVector("/completely/another/path/to/picture.eps");
    QVector<Models::ArtworkMetadata*> items;
    items << &metadataWithVector1 << &metadataWithVector2;

    QStringList filePathes, zipPathes;
    Conectivity::extractFilePathes(items, filePathes, zipPathes);

    QStringList expectedFiles, expectedZips;
    expectedFiles << "/path/to/picture.jpg"
                  << "/path/to/picture.eps"
                  << "/completely/another/path/to/picture.jpg"
                  << "/completely/another/path/to/picture.eps";

    expectedZips << "/path/to/picture.zip"
                 << "/completely/another/path/to/picture.zip";

    QCOMPARE(filePathes, expectedFiles);
    QCOMPARE(zipPathes, expectedZips);
}
