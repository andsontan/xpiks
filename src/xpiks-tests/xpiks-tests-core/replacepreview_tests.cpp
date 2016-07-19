#include "replacepreview_tests.h"
#include "../../xpiks-qt/Helpers/stringhelper.h"
#include"../../xpiks-qt/Common/defines.h"
#include "stringhelpersfortests.h"

void ReplacePreviewTests::noTructationTest() {
    QString textTest = getRandomString(PREVIEWOFFSET/4 + 1);
    const QString keyword = "keyword";
    textTest += keyword;
    textTest += getRandomString(PREVIEWOFFSET/4 + 1);
    QVERIFY(textTest.size() < 2*PREVIEWOFFSET);

    std::vector<int> hits;
    int pos = textTest.indexOf(keyword);
    hits.push_back(pos);

    QString result = Helpers::getReplacementSubstrings(textTest, hits, keyword.size());
    QCOMPARE(result, textTest);
}

void ReplacePreviewTests::simpleTructationTest() {
    QString textTest = getRandomString(PREVIEWOFFSET/4 + 1);
    const QString keyword = "keyword";
    textTest += keyword;
    textTest += getRandomString(2*PREVIEWOFFSET);
    QVERIFY(textTest.size() > 2*PREVIEWOFFSET);

    std::vector<int> hits;
    int pos = textTest.indexOf(keyword);
    hits.push_back(pos);
    QString result = Helpers::getReplacementSubstrings(textTest, hits, keyword.size());
    QString gold = textTest.mid(std::max(pos - PREVIEWOFFSET, 0), 2*PREVIEWOFFSET);

    QCOMPARE(result,gold);
}

void ReplacePreviewTests::doubleTructationTest() {
    QString textTest = getRandomString(PREVIEWOFFSET/4 + 1);
    const QString keyword = "keyword";
    textTest += keyword;
    textTest += getRandomString(2*PREVIEWOFFSET);
    textTest += keyword;
    QVERIFY(textTest.size()>2*PREVIEWOFFSET);

    std::vector<int> hits;
    int pos = 0;
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);
    pos += keyword.size();
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);

    QString result = Helpers::getReplacementSubstrings(textTest,hits, keyword.size());
    QString gold = textTest.mid(std::max(hits[0] - PREVIEWOFFSET, 0), 2*PREVIEWOFFSET) +
            textTest.mid(hits[1] - PREVIEWOFFSET, 2*PREVIEWOFFSET);

    QCOMPARE(result, gold);
}

void ReplacePreviewTests::advancedTructationTest() {
    QString textTest = getRandomString(PREVIEWOFFSET/4 + 1);
    const QString keyword = "keyword";
    textTest += keyword;
    textTest += getRandomString(2*PREVIEWOFFSET - keyword.size() - (PREVIEWOFFSET/4 + 1) - 4);
    textTest += keyword;
    QVERIFY(textTest.size() > 2*PREVIEWOFFSET);

    std::vector<int> hits;
    int pos = 0;
    pos = textTest.indexOf(keyword, pos);
    hits.push_back(pos);
    pos += keyword.size();
    pos = textTest.indexOf(keyword, pos);
    hits.push_back(pos);

    QString result = Helpers::getReplacementSubstrings(textTest, hits, keyword.size());
    int start = std::max(hits[0] - PREVIEWOFFSET, 0);
    QString gold = textTest.mid(start, hits[1] - start) + keyword;
    QCOMPARE(result, gold);
}

