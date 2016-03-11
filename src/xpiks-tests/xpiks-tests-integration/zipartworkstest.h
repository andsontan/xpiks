#ifndef ZIPARTWORKSTEST_H
#define ZIPARTWORKSTEST_H

#include "integrationtestbase.h"

class ZipArtworksTest : public IntegrationTestBase
{
public:
    ZipArtworksTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // ZIPARTWORKSTEST_H
