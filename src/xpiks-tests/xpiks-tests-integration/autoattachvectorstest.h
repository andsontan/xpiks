#ifndef AUTOATTACHVECTORSTEST_H
#define AUTOATTACHVECTORSTEST_H

#include "integrationtestbase.h"

namespace Commands {
    class CommandManager;
}


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
    virtual void teardown();
};

#endif // AUTOATTACHVECTORSTEST_H
