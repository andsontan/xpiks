#ifndef CLEARMETADATATEST_H
#define CLEARMETADATATEST_H

#include "integrationtestbase.h"

class ClearMetadataTest : public IntegrationTestBase
{
public:
    ClearMetadataTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // CLEARMETADATATEST_H
