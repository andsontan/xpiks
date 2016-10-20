#include "basicmetadatamodel_tests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/Common/basicmetadatamodel.h"
#include "../../xpiks-qt/Common/flags.h"

void BasicKeywordsModelTests::constructEmptyTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QCOMPARE(basicModel.isDescriptionEmpty(), true);
    QCOMPARE(basicModel.isTitleEmpty(), true);
    QCOMPARE(basicModel.isEmpty(), true);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QVERIFY(basicModel.getSpellCheckInfo() == NULL);
}

void BasicKeywordsModelTests::simpleAddKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::addExistingKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, false);

    QCOMPARE(addSignalSpy.count(), 0);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
}

void BasicKeywordsModelTests::addSeveralKeywordsTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, keywords.length());

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), keywords.length());
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), keywords.length() - 1);
}

void BasicKeywordsModelTests::removeExistingKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString keyword = "test keyword";
    int appendedCount = basicModel.appendKeyword(keyword);
    QCOMPARE(appendedCount, 1);

    QSignalSpy removeSignalSpy(&basicModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString removedKeyword;
    bool result = basicModel.takeKeywordAt(0, removedKeyword);
    QCOMPARE(result, true);

    QCOMPARE(removedKeyword, keyword);

    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QCOMPARE(removeSignalSpy.count(), 1);
    QList<QVariant> removedArguments = removeSignalSpy.takeFirst();
    QCOMPARE(removedArguments.at(1).toInt(), 0);
    QCOMPARE(removedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::removeNonExistingKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    bool result = basicModel.appendKeyword("test keyword");
    QCOMPARE(result, true);

    QSignalSpy removeSignalSpy(&basicModel, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QString removedKeyword;
    result = basicModel.takeKeywordAt(-1, removedKeyword);
    QCOMPARE(result, false);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(removeSignalSpy.count(), 0);

    result = basicModel.takeKeywordAt(1, removedKeyword);
    QCOMPARE(result, false);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(removeSignalSpy.count(), 0);
}

void BasicKeywordsModelTests::appendSameKeywordsTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QString keyword = "test keyword";
    QStringList keywords;
    keywords << keyword << keyword << keyword << keyword;

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 1);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::appendSameChangedKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;
    keywords << "keyword" << "KEYWORD" << "kEyworD" << "keYword" << "  keyword" << "keyword  " << "   kEyworD  ";

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 1);

    QCOMPARE(addSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QList<QVariant> addedArguments = addSignalSpy.takeFirst();
    QCOMPARE(addedArguments.at(1).toInt(), 0);
    QCOMPARE(addedArguments.at(2).toInt(), 0);
}

void BasicKeywordsModelTests::appendNoKeywordsTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QSignalSpy addSignalSpy(&basicModel, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QStringList keywords;

    int appendedCount = basicModel.appendKeywords(keywords);
    QCOMPARE(appendedCount, 0);

    QCOMPARE(addSignalSpy.count(), 0);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
}

void BasicKeywordsModelTests::clearKeywordsTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    QString description = "Some description";
    QString title = "Some title";
    basicModel.setDescription(description);
    basicModel.setTitle(title);
    basicModel.appendKeywords(keywords);

    QSignalSpy clearSignalSpy(&basicModel, SIGNAL(modelReset()));

    basicModel.clearKeywords();

    QCOMPARE(clearSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QCOMPARE(basicModel.getTitle(), title);
    QCOMPARE(basicModel.getDescription(), description);
}

void BasicKeywordsModelTests::clearModelTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";

    QString description = "Some description";
    QString title = "Some title";
    basicModel.setDescription(description);
    basicModel.setTitle(title);
    basicModel.appendKeywords(keywords);

    QSignalSpy clearSignalSpy(&basicModel, SIGNAL(modelReset()));

    basicModel.clearModel();

    QCOMPARE(clearSignalSpy.count(), 1);
    QCOMPARE(basicModel.getKeywordsCount(), 0);
    QVERIFY(basicModel.isDescriptionEmpty());
    QVERIFY(basicModel.isTitleEmpty());
}

void BasicKeywordsModelTests::containsKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.containsKeyword("keyword1"));
}

void BasicKeywordsModelTests::containsKeywordStrictTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "something_keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    Common::SearchFlags flags = Common::SearchFlags::ExactMatch;
    QVERIFY(basicModel.containsKeyword("keyword1", flags) == false);
    QVERIFY(basicModel.containsKeyword("keyword2", flags) == true);
    QVERIFY(basicModel.containsKeyword("kEyworD3", flags) == false);
}

void BasicKeywordsModelTests::containsKeywordFuzzyTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "something_keyword1" << "keyword2" << "KeyworD3";
    basicModel.appendKeywords(keywords);

    Common::SearchFlags notFuzzyMatchFlags = Common::SearchFlags::Keywords;
    QVERIFY(basicModel.containsKeyword("keyword1", notFuzzyMatchFlags) == true);
    QVERIFY(basicModel.containsKeyword("keyword2", notFuzzyMatchFlags) == true);
    QVERIFY(basicModel.containsKeyword("keyword3", notFuzzyMatchFlags) == true);
}

void BasicKeywordsModelTests::doesNotContainKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    auto notFuzzyMatchFlags = Common::SearchFlags::ExactKeywords;
    auto fuzzyMatchFlags = Common::SearchFlags::Keywords;
    QVERIFY(basicModel.containsKeyword("keyword4", fuzzyMatchFlags) == false);
    QVERIFY(basicModel.containsKeyword("keyword4", notFuzzyMatchFlags) == false);
    QVERIFY(basicModel.containsKeyword("keyword11", fuzzyMatchFlags) == false);
    QVERIFY(basicModel.containsKeyword("keyword11", notFuzzyMatchFlags) == false);
}

void BasicKeywordsModelTests::setSameTitleTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);
    bool result = basicModel.setTitle("new title");
    QCOMPARE(result, true);
    result = basicModel.setTitle("new title");
    QCOMPARE(result, false);
}

void BasicKeywordsModelTests::setSameDescriptionTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);
    bool result = basicModel.setDescription("new description");
    QCOMPARE(result, true);
    result = basicModel.setDescription("new description");
    QCOMPARE(result, false);
}

void BasicKeywordsModelTests::editKeywordToAnotherTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QSignalSpy modifiedSpy(&basicModel, SIGNAL(dataChanged(QModelIndex, QModelIndex,QVector<int>)));

    QVERIFY(basicModel.canEditKeyword(0, "keyword4"));
    bool editResult = basicModel.editKeyword(0, "keyword4");
    QCOMPARE(editResult, true);
    QCOMPARE(modifiedSpy.count(), 1);
}

void BasicKeywordsModelTests::editKeywordToSameTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QSignalSpy modifiedSpy(&basicModel, SIGNAL(dataChanged(QModelIndex, QModelIndex,QVector<int>)));

    QVERIFY(!basicModel.canEditKeyword(0, "keyword3"));
    bool editResult = basicModel.editKeyword(0, "keyword3");
    QCOMPARE(editResult, false);
    QCOMPARE(modifiedSpy.count(), 0);
}

void BasicKeywordsModelTests::editKeywordAnotherCaseTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "Mountain";
    basicModel.appendKeywords(keywords);

    QSignalSpy modifiedSpy(&basicModel, SIGNAL(dataChanged(QModelIndex, QModelIndex,QVector<int>)));

    bool editResult = basicModel.editKeyword(0, "mountain");
    QCOMPARE(editResult, true);
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("mountain"));
    QCOMPARE(modifiedSpy.count(), 1);
}

void BasicKeywordsModelTests::addRemoveAddUpperCaseWordTest() {
    const QString keyword = "Test";

    Common::BasicMetadataModel basicModel(m_FakeHold);

    bool addResult = basicModel.appendKeyword(keyword);
    QCOMPARE(addResult, true);

    QString removed;
    bool removeResult = basicModel.takeKeywordAt(0, removed);
    QCOMPARE(removeResult, true);
    QCOMPARE(removed, keyword);

    bool addAgainResult = basicModel.appendKeyword(keyword);
    QCOMPARE(addAgainResult, true);
}

void BasicKeywordsModelTests::editToUpperCaseTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.canEditKeyword(0, "Keyword1"));
    bool editSelfResult = basicModel.editKeyword(0, "Keyword1");
    QCOMPARE(editSelfResult, true);

    QVERIFY(!basicModel.canEditKeyword(1, "Keyword1"));
    bool editOtherResult = basicModel.editKeyword(1, "Keyword1");
    QCOMPARE(editOtherResult, false);
}

void BasicKeywordsModelTests::hasKeywordTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList keywords;
    keywords << "keyword1" << "keyword2" << "keyword3";
    basicModel.appendKeywords(keywords);

    QVERIFY(basicModel.hasKeyword("keyword1"));
    QVERIFY(basicModel.hasKeyword("keYwoRd1  "));
    QVERIFY(basicModel.hasKeyword("   kEyword1"));
    QVERIFY(basicModel.hasKeyword("KEYWORD1"));

    QVERIFY(!basicModel.hasKeyword("keyword1+"));
    QVERIFY(!basicModel.hasKeyword("keyword4+"));
}

void BasicKeywordsModelTests::simpleReplaceTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    Common::SearchFlags flags = Common::SearchFlags::Metadata;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("Replaced keyword1"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyword3 Replaced"));
}

void BasicKeywordsModelTests::descriptionReplaceTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    Common::SearchFlags flags = Common::SearchFlags::Description;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), originalTitle);
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), originalKeywords[1]);
    QCOMPARE(basicModel.getKeywordAt(2), originalKeywords[2]);
}

void BasicKeywordsModelTests::titleReplaceTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    Common::SearchFlags flags = Common::SearchFlags::Title;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), originalDescription);
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), originalKeywords[1]);
    QCOMPARE(basicModel.getKeywordAt(2), originalKeywords[2]);
}

void BasicKeywordsModelTests::keywordsReplaceTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    Common::SearchFlags flags = Common::SearchFlags::Keywords;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), originalTitle);
    QCOMPARE(basicModel.getDescription(), originalDescription);
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("Replaced keyword1"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyword3 Replaced"));
}

void BasicKeywordsModelTests::noReplaceCaseSensitiveTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    Common::SearchFlags flags = Common::SearchFlags::MetadataCaseSensitive;

    bool replaceSucceeded = basicModel.replace("test", "Replaced", flags);
    QVERIFY(!replaceSucceeded);
}

void BasicKeywordsModelTests::replaceCaseSensitiveTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title test here";
    QString originalDescription = "Test description test";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywtestord2" << "keyTestword3 test";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    auto flags = Common::SearchFlags::MetadataCaseSensitive;

    bool replaceSucceeded = basicModel.replace("test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Test title Replaced here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Test description Replaced"));
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords[0]);
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyTestword3 Replaced"));
}

void BasicKeywordsModelTests::replaceWholeWordsTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title test here";
    QString originalDescription = "Testdescription test";
    const QStringList originalKeywords(QStringList() << "Test keyword1" << "keywTestord2" << "keyTestword3 test");

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    auto flags = Common::SearchFlags::Metadata | Common::SearchFlags::ExactMatch;

    bool replaceSucceeded = basicModel.replace("test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title Replaced here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Testdescription Replaced"));
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("Replaced keyword1"));
    QCOMPARE(basicModel.getKeywordAt(1), originalKeywords[1]);
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("keyTestword3 Replaced"));
}

void BasicKeywordsModelTests::replaceKeywordsWithRemoveTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QString originalTitle = "Test title here";
    QString originalDescription = "Test description";
    QStringList originalKeywords;
    originalKeywords << "Test keyword1" << "keywTestord2" << "keyword3 Test" << "Replaced keyword1";

    basicModel.setTitle(originalTitle);
    basicModel.setDescription(originalDescription);
    basicModel.appendKeywords(originalKeywords);

    auto flags = Common::SearchFlags::Metadata;

    bool replaceSucceeded = basicModel.replace("Test", "Replaced", flags);
    QVERIFY(replaceSucceeded);
    QCOMPARE(basicModel.getTitle(), QLatin1String("Replaced title here"));
    QCOMPARE(basicModel.getDescription(), QLatin1String("Replaced description"));
    QCOMPARE(basicModel.getKeywordsCount(), originalKeywords.length() - 1);
    QCOMPARE(basicModel.getKeywordAt(0), QLatin1String("keywReplacedord2"));
    QCOMPARE(basicModel.getKeywordAt(1), QLatin1String("keyword3 Replaced"));
    QCOMPARE(basicModel.getKeywordAt(2), QLatin1String("Replaced keyword1"));
}

void BasicKeywordsModelTests::removeKeywordsFromSetTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList originalKeywords;
    originalKeywords << "keyword1" << "keyword2" << "keyword3 Test";
    basicModel.appendKeywords(originalKeywords);
    const bool caseSensitive = false;

    bool removed = basicModel.removeKeywords(QSet<QString>() << "keyword1" << "keyword2" << "keyword3", caseSensitive);
    QVERIFY(removed);

    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(basicModel.getKeywordAt(0), originalKeywords.back());
}

void BasicKeywordsModelTests::noneKeywordsRemovedFromSetTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList originalKeywords;
    originalKeywords << "akeyword1" << "keyword2_" << "keyword3 Test";
    basicModel.appendKeywords(originalKeywords);
    const bool caseSensitive = false;

    bool removed = basicModel.removeKeywords(QSet<QString>() << "keyword1" << "keyword2" << "keyword3", caseSensitive);
    QVERIFY(!removed);

    QCOMPARE(basicModel.getKeywordsCount(), originalKeywords.length());
}

void BasicKeywordsModelTests::removeKeywordsCaseSensitiveTest() {
    Common::BasicMetadataModel basicModel(m_FakeHold);

    QStringList originalKeywords;
    originalKeywords << "Keyword1" << "keyworD2" << "keyWord3";
    basicModel.appendKeywords(originalKeywords);
    const bool caseSensitive = true;

    bool removed = basicModel.removeKeywords(QSet<QString>() << "keyword1" << "keyword2" << "keyWord3", caseSensitive);
    QVERIFY(removed);

    QCOMPARE(basicModel.getKeywordsCount(), originalKeywords.length() - 1);
}

