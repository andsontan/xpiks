#ifndef AUTODETACHVECTORTEST_H
#define AUTODETACHVECTORTEST_H

#include "integrationtestbase.h"

class AutoDetachVectorTest : public IntegrationTestBase
{
public:
    AutoDetachVectorTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // AUTODETACHVECTORTEST_H
