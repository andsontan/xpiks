#ifndef SPELLCHECKMULTIREPLACETEST_H
#define SPELLCHECKMULTIREPLACETEST_H

#include "integrationtestbase.h"

class SpellCheckMultireplaceTest : public IntegrationTestBase
{
public:
    SpellCheckMultireplaceTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SPELLCHECKMULTIREPLACETEST_H
