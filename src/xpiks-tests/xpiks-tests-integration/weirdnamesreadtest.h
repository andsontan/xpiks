#ifndef WEIRDNAMESREADTEST_H
#define WEIRDNAMESREADTEST_H

#include "integrationtestbase.h"

class WeirdNamesReadTest : public IntegrationTestBase
{
public:
    WeirdNamesReadTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // WEIRDNAMESREADTEST_H
