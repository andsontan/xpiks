#include "artworkfilter_tests.h"
#include "Mocks/artworkmetadatamock.h"
#include "../../xpiks-qt/Helpers/filterhelpers.h"

void ArtworkFilterTests::searchImageVectorTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    QVERIFY(Helpers::hasSearchMatch("x:image", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("x:vector", &metadata, flagsAnyTermWithFilepath()));

    metadata.attachVector("/path/to/file.eps");
    QVERIFY(!Helpers::hasSearchMatch("x:image", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(Helpers::hasSearchMatch("x:vector", &metadata, flagsAnyTermWithFilepath()));
}

void ArtworkFilterTests::searchByKeywordsTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setKeywords(QStringList() << "keyword" << "another" << "test");
    QVERIFY(Helpers::hasSearchMatch("keyw TSTS", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("!keyw", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(Helpers::hasSearchMatch("!keyword", &metadata, flagsAnyTermWithFilepath()));

    QVERIFY(Helpers::hasSearchMatch("keyword super", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("keyword super", &metadata, flagsAllTermsWithoutFilepath()));
}

void ArtworkFilterTests::searchByTitleTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setTitle("my long title here");
    QVERIFY(Helpers::hasSearchMatch("tit", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("!tit", &metadata, flagsAnyTermWithFilepath()));
    // strict search works only for keywords
    QVERIFY(!Helpers::hasSearchMatch("!title", &metadata, flagsAnyTermWithFilepath()));

    QVERIFY(!Helpers::hasSearchMatch("keyword super", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(Helpers::hasSearchMatch("here my", &metadata, flagsAllTermsWithoutFilepath()));
}

void ArtworkFilterTests::searchByDescriptionTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setDescription("my long desciption john");
    QVERIFY(Helpers::hasSearchMatch("o", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("!o", &metadata, flagsAnyTermWithFilepath()));
    // strict search works only for keywords
    QVERIFY(!Helpers::hasSearchMatch("!description", &metadata, flagsAnyTermWithFilepath()));

    QVERIFY(!Helpers::hasSearchMatch("myjohn", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("descriptionmy", &metadata, flagsAllTermsWithoutFilepath()));
}

void ArtworkFilterTests::searchByFilepathTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    QVERIFY(Helpers::hasSearchMatch("file.jpg", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("file.jpg", &metadata, flagsAllTermsWithoutFilepath()));
}

void ArtworkFilterTests::strictSearchTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setKeywords(QStringList() << "keyword" << "ano!ther" << "test" << "k");
    QVERIFY(Helpers::hasSearchMatch("keyw", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(!Helpers::hasSearchMatch("!keyw", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(Helpers::hasSearchMatch("!keyword", &metadata, flagsAnyTermWithFilepath()));

    QVERIFY(!Helpers::hasSearchMatch("!!", &metadata, flagsAnyTermWithFilepath()));
    QVERIFY(Helpers::hasSearchMatch("!k", &metadata, flagsAllTermsWithoutFilepath()));

    metadata.appendKeyword("!");
    QVERIFY(Helpers::hasSearchMatch("!", &metadata, flagsAnyTermWithFilepath()));
}

void ArtworkFilterTests::searchWithSpaceTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");

    metadata.setTitle("A vector here");
    metadata.setDescription("Vector there");
    auto flags = Common::SearchFlags::IncludeSpaces | Common::SearchFlags::Metadata;

    QVERIFY(Helpers::hasSearchMatch(" vector ", &metadata, flags));

    metadata.setTitle("");
    QVERIFY(!Helpers::hasSearchMatch(" vector ", &metadata, flags));
}

void ArtworkFilterTests::caseSensitiveKeywordSearchTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setKeywords(QStringList() << "keYwOrd" << "keYword");

    auto flags = Common::SearchFlags::Keywords | Common::SearchFlags::CaseSensitive;

    QVERIFY(Helpers::hasSearchMatch("YwO", &metadata, flags));
    QVERIFY(!Helpers::hasSearchMatch("ywO", &metadata, flags));
}

void ArtworkFilterTests::cantFindWithFilterDescriptionTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setDescription("token between here");
    metadata.setKeywords(QStringList() << "some keyword" << "another stuff");

    auto flags = Common::SearchFlags::Keywords | Common::SearchFlags::Title;

    QVERIFY(!Helpers::hasSearchMatch("between", &metadata, flags));
    QVERIFY(Helpers::hasSearchMatch("between", &metadata, flags | Common::SearchFlags::Description));
}

void ArtworkFilterTests::cantFindWithFilterTitleTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setTitle("token between here");
    metadata.setKeywords(QStringList() << "some keyword" << "another stuff");

    auto flags = Common::SearchFlags::Description | Common::SearchFlags::Keywords;

    QVERIFY(!Helpers::hasSearchMatch("between", &metadata, flags));
    QVERIFY(Helpers::hasSearchMatch("between", &metadata, flags | Common::SearchFlags::Title));
}

void ArtworkFilterTests::cantFindWithFilterKeywordsTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setDescription("another keyword in description");
    metadata.setTitle("token between here");
    metadata.setKeywords(QStringList() << "some keyword" << "another stuff");

    auto flags = Common::SearchFlags::Description | Common::SearchFlags::Title;

    QVERIFY(!Helpers::hasSearchMatch("stuff", &metadata, flags));
    QVERIFY(Helpers::hasSearchMatch("stuff", &metadata, flags | Common::SearchFlags::Keywords));
}

void ArtworkFilterTests::cantFindWithFilterSpecialTest() {
    Mocks::ArtworkMetadataMock metadata("/path/to/file.jpg");
    metadata.setDescription("another keyword in description");
    metadata.setTitle("token between here");
    metadata.setKeywords(QStringList() << "some keyword" << "another stuff");

    auto flags = Common::SearchFlags::Description | Common::SearchFlags::Title | Common::SearchFlags::Keywords;

    QVERIFY(!Helpers::hasSearchMatch("x:modified", &metadata, flags));
    QVERIFY(Helpers::hasSearchMatch("x:modified", &metadata, flags | Common::SearchFlags::ReservedTerms));
}
