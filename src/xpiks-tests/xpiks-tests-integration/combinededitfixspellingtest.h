#ifndef COMBINEDEDITFIXSPELLINGTEST_H
#define COMBINEDEDITFIXSPELLINGTEST_H

#include "integrationtestbase.h"

class CombinedEditFixSpellingTest : public IntegrationTestBase
{
public:
    CombinedEditFixSpellingTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // COMBINEDEDITFIXSPELLINGTEST_H
