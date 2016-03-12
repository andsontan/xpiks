#ifndef AUTOATTACHVECTORSTEST_H
#define AUTOATTACHVECTORSTEST_H

#include "integrationtestbase.h"

class AutoAttachVectorsTest : public IntegrationTestBase
{
public:
    AutoAttachVectorsTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // AUTOATTACHVECTORSTEST_H
