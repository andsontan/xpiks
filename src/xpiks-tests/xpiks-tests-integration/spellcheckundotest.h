#ifndef SPELLCHECKUNDOTEST_H
#define SPELLCHECKUNDOTEST_H

#include "integrationtestbase.h"

class SpellCheckUndoTest : public IntegrationTestBase
{
public:
    SpellCheckUndoTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SPELLCHECKUNDOTEST_H
