#ifndef ADDFILESBASICTEST_H
#define ADDFILESBASICTEST_H

#include "integrationtestbase.h"

namespace Commands {
    class CommandManager;
}

class AddFilesBasicTest : public IntegrationTestBase
{
public:
    AddFilesBasicTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
    virtual void teardown();
};

#endif // ADDFILESBASICTEST_H
