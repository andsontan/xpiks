#include "artworkfilter_tests.h"
#include "Mocks/artworkmetadatamock.h"
#include "../../xpiks-qt/Helpers/filterhelpers.h"

void ArtworkFilterTests::searchImageVectorTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    QVERIFY(Helpers::containsPartsSearch("x:image", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("x:vector", &metadata, false));

    metadata.attachVector("/path/to/file.eps");
    QVERIFY(!Helpers::containsPartsSearch("x:image", &metadata, false));
    QVERIFY(Helpers::containsPartsSearch("x:vector", &metadata, false));
}

void ArtworkFilterTests::searchByKeywordsTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setKeywords(QStringList() << "keyword" << "another" << "test");
    QVERIFY(Helpers::containsPartsSearch("keyw TSTS", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("!keyw", &metadata, false));
    QVERIFY(Helpers::containsPartsSearch("!keyword", &metadata, false));

    QVERIFY(Helpers::containsPartsSearch("keyword super", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("keyword super", &metadata, true));
}

void ArtworkFilterTests::searchByTitleTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setTitle("my long title here");
    QVERIFY(Helpers::containsPartsSearch("tit", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("!tit", &metadata, false));
    // strict search works only for keywords
    QVERIFY(!Helpers::containsPartsSearch("!title", &metadata, false));

    QVERIFY(!Helpers::containsPartsSearch("keyword super", &metadata, false));
    QVERIFY(Helpers::containsPartsSearch("here my", &metadata, true));
}

void ArtworkFilterTests::searchByDescriptionTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setDescription("my long desciption john");
    QVERIFY(Helpers::containsPartsSearch("o", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("!o", &metadata, false));
    // strict search works only for keywords
    QVERIFY(!Helpers::containsPartsSearch("!description", &metadata, false));

    QVERIFY(!Helpers::containsPartsSearch("myjohn", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("descriptionmy", &metadata, true));
}

void ArtworkFilterTests::strictSearchTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setKeywords(QStringList() << "keyword" << "ano!ther" << "test" << "k");
    QVERIFY(Helpers::containsPartsSearch("keyw", &metadata, false));
    QVERIFY(!Helpers::containsPartsSearch("!keyw", &metadata, false));
    QVERIFY(Helpers::containsPartsSearch("!keyword", &metadata, false));

    QVERIFY(!Helpers::containsPartsSearch("!!", &metadata, false));
    QVERIFY(Helpers::containsPartsSearch("!k", &metadata, true));

    metadata.appendKeyword("!");
    QVERIFY(Helpers::containsPartsSearch("!", &metadata, false));
}
