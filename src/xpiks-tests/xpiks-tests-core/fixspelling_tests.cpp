#include "fixspelling_tests.h"
#include <QSignalSpy>
#include "../../xpiks-qt/SpellCheck/spellchecksuggestionmodel.h"
#include "../../xpiks-qt/Common/basickeywordsmodel.h"
#include "../../xpiks-qt/Common/flags.h"
#include "Mocks/commandmanagermock.h"
#include "../../xpiks-qt/SpellCheck/spellsuggestionsitem.h"
#include "../../xpiks-qt/SpellCheck/spellcheckiteminfo.h"
#include "Mocks/spellcheckservicemock.h"

#define INIT_FIX_SPELLING_TEST \
    Mocks::CommandManagerMock commandManager; \
    Mocks::SpellCheckServiceMock spellCheckService; \
    commandManager.InjectDependency(&spellCheckService); \
    SpellCheck::SpellCheckSuggestionModel suggestionModel; \
    commandManager.InjectDependency(&suggestionModel); \
    Common::BasicKeywordsModel basicModel(m_FakeHold); \
    SpellCheck::SpellCheckItemInfo spellCheckInfo; \
    basicModel.setSpellCheckInfo(&spellCheckInfo);

void FixSpellingTests::fixKeywordsSmokeTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("title", "description", "keyword1, keyword2");
    basicModel.getSpellCheckResults()[0] = false;
    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::All);

    SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(0);
    QString itemToReplace = "item1";
    suggestionItem->setSuggestions(QStringList() << itemToReplace << "item2");
    suggestionItem->setReplacementIndex(0);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], itemToReplace);
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::noReplacementsSelectedTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("title", "description", "keyword1, keyword2");
    basicModel.getSpellCheckResults()[0] = false;

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::All);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("keyword1"));
    QVERIFY(spellCheckSpy.isEmpty());
}

void FixSpellingTests::fixAndRemoveDuplicatesTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("title", "description", "keyword1, keyword2");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[1] = false;

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::All);

    QCOMPARE(basicModel.getKeywordsCount(), 2);

    SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(0);
    suggestionItem->setReplacementIndex(0);

    SpellCheck::SpellSuggestionsItem *otherSuggestionItem = suggestionModel.getItem(1);
    otherSuggestionItem->setReplacementIndex(0);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("item1"));
    QCOMPARE(basicModel.getKeywordsCount(), 1);
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::fixAndRemoveDuplicatesCombinedTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("title", "description", "keyword1, keyword2 item1 test, keyword2 wordtoreplace test");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[2] = false;

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::Keywords);

    QCOMPARE(basicModel.getKeywordsCount(), 3);
    qDebug() << basicModel.getKeywords();
    QCOMPARE(suggestionModel.rowCount(), 4);

    for (int i = 0; i < suggestionModel.rowCount(); ++i) {
        SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(i);
        if (suggestionItem->getWord() == "wordtoreplace") {
            suggestionItem->setReplacementIndex(0);
        }
    }

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywordsCount(), 2);
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::multiReplaceWithCorrectAllTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("wordtoreplace in title", "description has wordtoreplace too", "wordtoreplace, keyword2, word plus wordtoreplace");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[2] = false;
    spellCheckInfo.setDescriptionErrors(QSet<QString>() << "wordtoreplace");
    spellCheckInfo.setTitleErrors(QSet<QString>() << "wordtoreplace");

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::All);

    QCOMPARE(suggestionModel.rowCount(), 3);
    for (int i = 0; i < suggestionModel.rowCount(); ++i) {
        SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(i);
        SpellCheck::CombinedSpellSuggestions *combinedItem = dynamic_cast<SpellCheck::CombinedSpellSuggestions*>(suggestionItem);
        if (combinedItem != NULL) {
            suggestionItem->setReplacementIndex(0);
        }
    }

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("item1"));
    QCOMPARE(basicModel.getKeywords()[2], QString("word plus item1"));
    QCOMPARE(basicModel.getDescription(), QString("description has item1 too"));
    QCOMPARE(basicModel.getTitle(), QString("item1 in title"));
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::replaceWithCorrectDescriptionTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("wordtoreplace in title", "description has wordtoreplace too", "wordtoreplace, keyword2, word plus wordtoreplace");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[2] = false;
    spellCheckInfo.setDescriptionErrors(QSet<QString>() << "wordtoreplace");
    spellCheckInfo.setTitleErrors(QSet<QString>() << "wordtoreplace");

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::Description);

    QCOMPARE(suggestionModel.rowCount(), 1);
    SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(0);
    suggestionItem->setReplacementIndex(0);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("wordtoreplace"));
    QCOMPARE(basicModel.getKeywords()[2], QString("word plus wordtoreplace"));
    QCOMPARE(basicModel.getDescription(), QString("description has item1 too"));
    QCOMPARE(basicModel.getTitle(), QString("wordtoreplace in title"));
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::replaceWithCorrectTitleTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("wordtoreplace in title", "description has wordtoreplace too", "wordtoreplace, keyword2, word plus wordtoreplace");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[2] = false;
    spellCheckInfo.setDescriptionErrors(QSet<QString>() << "wordtoreplace");
    spellCheckInfo.setTitleErrors(QSet<QString>() << "wordtoreplace");

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::Title);

    QCOMPARE(suggestionModel.rowCount(), 1);
    SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(0);
    suggestionItem->setReplacementIndex(0);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("wordtoreplace"));
    QCOMPARE(basicModel.getKeywords()[2], QString("word plus wordtoreplace"));
    QCOMPARE(basicModel.getDescription(), QString("description has wordtoreplace too"));
    QCOMPARE(basicModel.getTitle(), QString("item1 in title"));
    QCOMPARE(spellCheckSpy.count(), 1);
}

void FixSpellingTests::replaceWithCorrectKeywordsTest() {
    INIT_FIX_SPELLING_TEST;

    QSignalSpy spellCheckSpy(&basicModel, SIGNAL(spellCheckErrorsChanged()));

    basicModel.initialize("wordtoreplace in title", "description has wordtoreplace too", "wordtoreplace, keyword2, word plus wordtoreplace");
    basicModel.getSpellCheckResults()[0] = false;
    basicModel.getSpellCheckResults()[2] = false;
    spellCheckInfo.setDescriptionErrors(QSet<QString>() << "wordtoreplace");
    spellCheckInfo.setTitleErrors(QSet<QString>() << "wordtoreplace");

    suggestionModel.setupModel(&basicModel, 0, Common::SuggestionFlags::Keywords);

    QCOMPARE(suggestionModel.rowCount(), 3);
    for (int i = 0; i < suggestionModel.rowCount(); ++i) {
        SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(i);
        SpellCheck::CombinedSpellSuggestions *combinedItem = dynamic_cast<SpellCheck::CombinedSpellSuggestions*>(suggestionItem);
        if (combinedItem != NULL) {
            suggestionItem->setReplacementIndex(0);
        }
    }

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], QString("item1"));
    QCOMPARE(basicModel.getKeywords()[2], QString("word plus item1"));
    QCOMPARE(basicModel.getDescription(), QString("description has wordtoreplace too"));
    QCOMPARE(basicModel.getTitle(), QString("wordtoreplace in title"));
    QCOMPARE(spellCheckSpy.count(), 1);
}
