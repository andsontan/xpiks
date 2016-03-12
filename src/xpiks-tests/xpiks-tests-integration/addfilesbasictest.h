#ifndef ADDFILESBASICTEST_H
#define ADDFILESBASICTEST_H

#include "integrationtestbase.h"

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
};

#endif // ADDFILESBASICTEST_H
