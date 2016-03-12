#ifndef SPELLCHECKCOMBINEDMODELTEST_H
#define SPELLCHECKCOMBINEDMODELTEST_H

#include "integrationtestbase.h"

class SpellCheckCombinedModelTest : public IntegrationTestBase
{
public:
    SpellCheckCombinedModelTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SPELLCHECKCOMBINEDMODELTEST_H
