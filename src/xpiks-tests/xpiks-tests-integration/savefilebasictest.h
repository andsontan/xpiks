#ifndef SAVEFILEBASICTEST_H
#define SAVEFILEBASICTEST_H

#include "integrationtestbase.h"

class SaveFileBasicTest : public IntegrationTestBase
{
public:
    SaveFileBasicTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SAVEFILEBASICTEST_H
