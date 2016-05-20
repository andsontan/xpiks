#include "fixspelling_tests.h"
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

    basicModel.initialize("title", "description", "keyword");
    basicModel.getSpellCheckResults()[0] = false;
    suggestionModel.setupModel(&basicModel, 0, Common::CorrectAll);

    SpellCheck::SpellSuggestionsItem *suggestionItem = suggestionModel.getItem(0);
    QString itemToReplace = "item1";
    suggestionItem->setSuggestions(QStringList() << itemToReplace << "item2");
    suggestionItem->setReplacementIndex(0);

    suggestionModel.submitCorrections();

    QCOMPARE(basicModel.getKeywords()[0], itemToReplace);
}
