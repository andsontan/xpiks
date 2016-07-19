#include "replacepreview.h"
#include "../../xpiks-qt/Models/findandreplacemodel.h"
#include"../../xpiks-qt/Common/defines.h"
#include <QString>

QString GetRandomString(int randomStringLength);

void ReplacePreviewTest::noTructation(){
    QString textTest = GetRandomString(PREVIEWOFFSET/4+1);
    QString keyword = "keyword";
    textTest+=keyword;
    textTest+=GetRandomString(PREVIEWOFFSET/4+1);
    Q_ASSERT(textTest.size()<2*PREVIEWOFFSET);
    std::vector<int> hits;
    int pos = textTest.indexOf(keyword);
    hits.push_back(pos);
    QString result = Models::FindAndReplaceModel::getSubstrings(textTest,hits, keyword.size());
    QCOMPARE(result, textTest);
}

void ReplacePreviewTest::simpleTructation(){
    QString textTest = GetRandomString(PREVIEWOFFSET/4+1);
    QString keyword = "keyword";
    textTest+=keyword;
    textTest+=GetRandomString(2*PREVIEWOFFSET);
    Q_ASSERT(textTest.size()>2*PREVIEWOFFSET);
    std::vector<int> hits;
    int pos = textTest.indexOf(keyword);
    hits.push_back(pos);
    QString result = Models::FindAndReplaceModel::getSubstrings(textTest,hits, keyword.size());
    QString gold = textTest.mid(std::max(pos-PREVIEWOFFSET,0),2*PREVIEWOFFSET);

    QCOMPARE(result,gold);
}

void ReplacePreviewTest::doubleTructation(){
    QString textTest = GetRandomString(PREVIEWOFFSET/4+1);
    QString keyword = "keyword";
    textTest+=keyword;
    textTest+=GetRandomString(2*PREVIEWOFFSET);
    textTest+=keyword;
    Q_ASSERT(textTest.size()>2*PREVIEWOFFSET);
    std::vector<int> hits;
    int pos = 0;
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);
    pos+=keyword.size();
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);
    QString result = Models::FindAndReplaceModel::getSubstrings(textTest,hits, keyword.size());
    QString gold = textTest.mid(std::max(hits[0]-PREVIEWOFFSET,0),2*PREVIEWOFFSET)+textTest.mid(hits[1]-PREVIEWOFFSET,2*PREVIEWOFFSET);
    QCOMPARE(result,gold);
}

void ReplacePreviewTest::advancedTructation(){
    QString textTest = GetRandomString(PREVIEWOFFSET/4+1);
    QString keyword = "keyword";
    textTest+=keyword;
    textTest+=GetRandomString(2*PREVIEWOFFSET-keyword.size()-PREVIEWOFFSET/4+1-4);
    textTest+=keyword;
    Q_ASSERT(textTest.size()>2*PREVIEWOFFSET);
    std::vector<int> hits;
    int pos = 0;
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);
    pos+=keyword.size();
    pos = textTest.indexOf(keyword,pos);
    hits.push_back(pos);
    QString result = Models::FindAndReplaceModel::getSubstrings(textTest,hits, keyword.size());
    int start = std::max(hits[0]-PREVIEWOFFSET,0);
    QString gold = textTest.mid(start,hits[1]-start)+keyword;
    QCOMPARE(result,gold);
}



QString GetRandomString(int randomStringLength){
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
