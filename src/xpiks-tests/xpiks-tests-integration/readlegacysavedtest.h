#ifndef READLEGACYSAVEDTEST_H
#define READLEGACYSAVEDTEST_H

#include "integrationtestbase.h"

class ReadLegacySavedTest: public IntegrationTestBase
{
public:
    ReadLegacySavedTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // READLEGACYSAVEDTEST_H
